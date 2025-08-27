#include <format>
#include <iostream>
#include <json.hpp>
#include <optional>
#include <program.hpp>
#include <utils.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

void Program::run() {
	loadLibrary();
	loadDownloaded();
	organizeSongs();

	downloadNeededSongs();
	deleteUnwantedSongs();

	std::cout << "Songs to delete:\n";
	for (auto &song : mSongsTodelete) {
		std::cout << *song << "\n";
	}
}

void Program::downloadNeededSongs() {}

Program::Program(const std::filesystem::path &musicPath,
                 const std::string &library)
    : mMusicPath(musicPath), mLibraryFile(library) {}

bool sureDifferentUrl(const Song &song, const std::string &originalUrl);

void Program::loadLibrary() {
	std::cout << "Loading library...\n";
	std::string line;
	std::string artist;

	uint lineNumber = 0;

	while (std::getline(mLibraryFile, line)) {
		lineNumber++;
		if (line.starts_with("//")) {
			continue;
		}

		if (auto temp = getArtist(line)) {
			artist = temp.value();
		}

		auto albumName = getAlbum(line);
		if (!albumName.has_value()) {
			continue;
		}

		Album::Ptr album = std::make_shared<Album>(albumName.value());
		album->artist = artist;

		if (auto thumbnail = getThumbnail(line)) {
			album->imageURL = thumbnail.value();
		}

		if (!(!((album->imageURL.length() == 0) ^
		        (album->artist.length() == 0)))) {
			throw std::runtime_error(
			    std::format("Did not found image url or the artist in line {}",
			                lineNumber));
		}

		int i = 1;
		while (line != "") {
			if (!std::getline(mLibraryFile, line) || line.empty()) {
				break;
			}

			lineNumber++;
			if (line.starts_with("//")) {
				continue;
			}

			auto songName = getName(line);
			Song::Ptr song = std::make_unique<Song>(
			    songName.value_or(""), album, Song::Status::Library);
			if (auto URL = getLink(line)) {
				if (URL->empty()) {
					std::cerr
					    << std::format("{} - {} Has no link, skipping over it",
					                   album->name, song->getName())
					    << "\n";
					i++;
					continue;
				}
				song->setURL(URL.value());
			}

			if (song->getName().length() > 0) {
				song->setTrackNumber(i++);
				album->songs.emplace_back(std::move(song));
			}
		}

		album->totalSize = album->songs.size();
		if (mLibrary[album->name].get() == nullptr) {
			mLibrary[album->name] = std::make_shared<Album>(album->name);
		}
		mLibrary[album->name] = std::move(album);
	}
	std::cout << "Finished loading library...\n";
}

void Program::loadDownloaded() {
	std::cout << "Loading downloaded songs...\n";
	for (const auto &pathIt : fs::directory_iterator(mMusicPath)) {
		if (!pathIt.is_regular_file()) {
			continue;
		}
		auto foundTemp = pathIt.path().string().find("temp");
		if (foundTemp != std::string::npos) {
			fs::remove(pathIt.path());
			continue;
		}

		FileMetadata metadata(pathIt.path().c_str());
		auto pathstr = pathIt.path().string();
		auto lastSlash = pathstr.find_last_of('/');
		auto name = std::string(pathstr.begin() + lastSlash + 1, pathstr.end());
		auto albumName = metadata.read("album");
		auto artistName = metadata.read("artist");
		// auto songUrl = metadata.read("");
		Album::Ptr album = std::make_unique<Album>(albumName);
		Song::Ptr song =
		    std::make_unique<Song>(name, album, Song::Status::Downloaded);
		// song->setURL(songUrl);
		album->artist = artistName;

		// mDownloaded[album->name] = std::move(album);
		if (mDownloaded[albumName].get() == nullptr) {
			mDownloaded[albumName] = std::make_shared<Album>(albumName);
		}
		mDownloaded[albumName]->songs.push_back(std::move(song));
	}
	std::cout << "Finished loading downloaded songs...\n";
}

bool sureDifferentUrl(const Song &song, const std::string &originalUrl) {
	std::cout << song.getName()
	          << ": are you sure you want to replace url?\noriginal: "
	          << originalUrl << "\nnew     : " << song.getURL();
	std::cout << "\ny/N\n";

	std::string answer;
	std::getline(std::cin, answer);

	return (answer == "y" || answer == "Y");
}

// TODO: create library class and foreach
void Program::organizeSongs() {
	mSongsTodelete = collectToDelete(mLibrary, mDownloaded);
}

void Program::changeUrls() {
	for (auto &song : mUrlToChange) {
		std::cout << song
		          << std::format("Are you sure you want to change \nOriginal: "
		                         "{} \nnew: {}",
		                         song->getURL(), song->getAlternateUrl());
		confirmUserInput([&]() {
			song->setURL(song->getAlternateUrl());
			mSongsToDownload.emplace_back(std::move(song));
			fs::remove(mMusicPath / song->toFile());
		});
	}
}

void Program::deleteUnwantedSongs() {
	if (mSongsTodelete.empty()) {
		std::cout << "No songs to delete! :D\n";
		return;
	}

	std::cout << "Are you sure you want to delete: \n";
	for (auto &song : mSongsTodelete) {
		std::cout << song << "\n";
	}
	std::cout << std::endl;

	confirmUserInput([&]() {
		for (auto &song : mSongsTodelete) {
			std::cout << "Deleteing: " << mMusicPath / song->toFile() << "\n";
			fs::remove(mMusicPath / song->toFile());
		}
	});
}
