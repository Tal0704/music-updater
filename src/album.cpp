#include <album.hpp>
#include <song.hpp>
#include <json.hpp>
#include <iostream>
#include <exec.hpp>
#include <string>

namespace fs = std::filesystem;
using json = nlohmann::json;

Album::Album() {

}

Album::Album(const std::string& name)
	: name(name)
{

}

std::string cURLCommand(const std::string& URL, const std::string& path) {
	std::string curlImage("curl -s --url \"");
	curlImage += URL;
	curlImage += "\" --output \"";
	curlImage += path;
	curlImage += "/temp.jpg\"";
	return curlImage;
}

void Album::download(const std::filesystem::path& path) {
	exec(cURLCommand(imageURL, path.string()));
	for(auto& song: songs) {
		std::cout << "Downloading: " << this->name << " - " << song->name << "...\n";
		song->download(path);
	}
	fs::remove(path.string() + "/temp.jpg");
}

std::string getSearchTerm(const Album& album)
{
	std::string searchTerm = album.name;
	searchTerm += album.artist;
	std::replace(searchTerm.begin(), searchTerm.end(), ' ', '+');
	return searchTerm;
}

std::string convertToUri(const char* str) {
	std::string uri = str;
	for(uint i = 0; i < uri.length(); i++) {
		if(uri[i] == ' ') {
			uri[i] = '%';
			uri.insert(i + 1, 1, '0');
			uri.insert(i + 1, 1, '2');
		}
	}
	return uri;
}

float calcPercent(float value, uint total) {
	return value / total;
}

float precentAccurate(const std::string& left, const std::string& right) {
	uint totalAccurate = 0;
	uint i = 0;
	for(i = 0; i < std::max(left.size(), right.size()); i++) {
		if(i >= left.size() || i >= right.size()) {
			return calcPercent(totalAccurate, i);
		}
	}
	return calcPercent(totalAccurate, i);
}

bool isIDhasThumbnail(const std::string& id) {
	auto a = (exec(std::string("curl -o /dev/null -s -w \"%{http_code}\n\" -L https://coverartarchive.org/release/") + id + "/front")) == "200\n";
	return a;
}

void Album::populateMetadata() {
	std::string curlCommand = "curl -s --request GET --url \"https://musicbrainz.org/ws/2/release/?query=artist:";
	curlCommand += convertToUri(artist.c_str());
	curlCommand += "%20AND%20release:";
	curlCommand += convertToUri(name.c_str());
	curlCommand += "&fmt=json\"";
	json data = json::parse(exec(curlCommand));

	json* correctAlbum = &data["releases"][0];

	for(auto& release: data["releases"]) {
		if(release["score"] <= 60)
			continue;
		std::string correctRawDate = correctAlbum->at("date").template get<std::string>();
		if(correctRawDate == "") 
			continue;

		int correctYear = std::stoi(correctRawDate.substr(0, 4));
		try {
			std::string currentRawDate = release.at("date").template get<std::string>();

			if(currentRawDate == "") 
				continue;

			int currentYear = std::stoi(currentRawDate.substr(0, 4));

			if(currentYear < correctYear) {
				correctAlbum = &release;
			}
		}
		catch (...) {
			continue;
		}
	}

	auto& album = *correctAlbum;
	std::string albumID = album["id"].template get<std::string>();
	year = album["date"].template get<std::string>().substr(0, 4);
	year = year.substr(0, 4);
	artist = album["artist-credit"][0]["name"].template get<std::string>();
	name = album["title"];
}
