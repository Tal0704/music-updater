#include <iostream>
#include <iterator>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <youtube.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

std::string extractId(const std::string &link) {
	auto posOfLastEqual = link.find_last_of('=');
	std::string id = link.substr(posOfLastEqual + 1);
	return id;
}

Youtube::Youtube(const fs::path &libPath, const std::string &link,
                 const std::string &apiKey)
    : mLibPath(libPath) {
	mPlaylistId = extractId(link);
	struct passwd *pw = getpwuid(getuid());
	mConfigFile = std::format("{}/.config/spoopify/config.json", pw->pw_dir);

	if (!apiKey.empty()) {
		mApiKey = apiKey;
	} else {
		loadApiKey();
	}
}

void Youtube::loadApiKey() {
	std::ifstream configFile(mConfigFile);

	json data;
	configFile >> data;

	mApiKey = data["apiKey"].template get<std::string>();
}

void Youtube::setApiKey(const std::string &key) {
	std::fstream configFile(mConfigFile);
	json data;
	configFile >> data;

	data["apiKey"] = key;
	configFile << data;
}

void Youtube::getVideos() {
	constexpr auto baseLink =
	    "https://www.googleapis.com/youtube/v3/playlistItems";

	auto apiLink =
	    std::format("{}?key={}&playlistId={}&part=snippet&maxResults=50",
	                baseLink, mApiKey, mPlaylistId);
	auto res = httpsGet(apiLink);

	if (!res.has_value()) {
		throw std::runtime_error(std::format("No respone from: {}", apiLink));
	}

	json data = json::parse(res.value());

	for (const auto &item : data["items"]) {
		mVideosLink[item["snippet"]["title"]] =
		    std::format("https://www.youtube.com/watch?v={}",
		                item["snippet"]["resourceId"]["videoId"]
		                    .template get<std::string>());
	}
}

void Youtube::getTitle() {
	constexpr auto baseLink = "https://www.googleapis.com/youtube/v3/playlists";

	auto apiLink = std::format("{}?key={}&id={}&part=snippet&maxResults=50",
	                           baseLink, mApiKey, mPlaylistId);
	auto res = httpsGet(apiLink);

	if (!res.has_value()) {
		throw std::runtime_error(std::format("No respone from: {}", apiLink));
	}

	json data = json::parse(res.value());

	mPlaylistTitle = data["items"][0]["snippet"]["title"];
}

typedef std::map<uint, std::string> dbType;
dbType loadArtists(const fs::path &path) {
	dbType database;
	std::ifstream library(path);
	std::string line;

	uint lineNumber = 0;
	while (std::getline(library, line)) {
		lineNumber++;
		if (line.starts_with("# ")) {
			database[lineNumber] = line.substr(2);
		}
	}

	return database;
}

std::string extractAlbum(const std::string &playlistTitle) {
	std::string album;

	auto hyphon = playlistTitle.find("-");
	if (hyphon != std::string::npos) {
		auto bracket =
		    std::find(playlistTitle.begin() + hyphon, playlistTitle.end(), '(');
		album =
		    playlistTitle.substr(hyphon + 1, bracket - playlistTitle.begin());
		return album;
	}

	return "";
}

std::optional<std::string> getArtist(const std::string &playlistTitle,
                                     const dbType &db) {
	std::string artistName;

	for (const auto &[lineNumber, artist] : db) {
		auto artistIt = playlistTitle.find(artist);
		if (artistIt != std::string::npos) {
			artistName = artist;
			extractAlbum(playlistTitle);
		}
	}

	if (artistName.empty()) {
		return {};
	}

	return artistName;
}

std::string formatAlbum(const std::string &playlistTitle,
                        const std::map<std::string, std::string> &map) {
	std::string output;
	output += "## [" + playlistTitle + "]()\n";
	for (const auto &[title, link] : map) {
		output += "[" + title + "](" + link + ")\n";
	}
	return output;
}

void Youtube::writeToFile() {
	auto database = loadArtists(mLibPath);
	std::string artist;
	std::string album;
	auto res = getArtist(mPlaylistTitle, database);
	if (res.has_value()) {
		artist = res.value();

		std::ifstream library(mLibPath, std::ios_base::ate);

		std::string buff;
		auto size = library.tellg();
		buff.resize(size);
		library.seekg(0, std::ios_base::beg);

		if (!library.read(buff.data(), size)) {
			throw std::runtime_error("couldn't read in file. Aborting!");
		}

		auto found = buff.find(artist);
		auto lineBreak = buff.find("\n", found);
		buff.insert(lineBreak, "\n");
		buff.insert(lineBreak + 1, formatAlbum(mPlaylistTitle, mVideosLink));

		std::ofstream(mLibPath) << buff;

	} else {
		std::ofstream ostream(mLibPath, std::ios::app);
		ostream << "\n\n" << formatAlbum(mPlaylistTitle, mVideosLink);
	}
}
