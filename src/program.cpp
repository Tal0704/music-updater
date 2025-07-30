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
	organizeSongs();
	for (auto &song : mSongsTodelete) {
		std::cout << *song << "\n";
	}
}

Program::Program(const std::filesystem::path &musicPath,
                 const std::string &library)
    : mMusicPath(musicPath), mLibraryFile(library) {}

std::optional<std::string> getName(const std::string &line);
std::optional<std::string> getThumbnail(const std::string &line);
std::optional<std::string> getLink(const std::string &line);
std::optional<std::string> getAlbum(const std::string &line);
std::optional<std::string> getArtist(const std::string &line);

bool sureDifferentUrl(const Song &song, const std::string &originalUrl);

void Program::loadLibrary() {
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
			if (!std::getline(mLibraryFile, line)) {
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
				song->setURL(URL.value_or(""));
			} else {
				std::cerr << std::format(
				                 "{} - {} Has no link, skipping over it",
				                 album->name, song->getName())
				          << "\n";
				i++;
				continue;
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
}

void Program::loadDownloaded() {
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
		auto songUrl = metadata.read("comment");
		Album::Ptr album = std::make_unique<Album>(albumName);
		Song::Ptr song =
		    std::make_unique<Song>(name, album, Song::Status::Downloaded);
		song->setURL(songUrl);
		album->artist = artistName;

		// TODO: Check if not make null reference
		// mDownloaded[album->name] = std::move(album);
		if (mDownloaded[albumName].get() == nullptr) {
			mDownloaded[albumName] = std::make_shared<Album>(albumName);
		}
		mDownloaded[albumName]->songs.push_back(std::move(song));
	}
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

// void collectMissing(
//     Library &source, Library &target, std::vector<Song::Ptr> destination,
//     std::function<void(const Song::Ptr &, const Song::Ptr &)> pred =
//         [](const Song::Ptr &, const Song::Ptr &) {}) {
// 	for (auto &song : source) {
// 		auto found =
// 		    std::find_if(target.begin(), target.end(), [&](auto &targetSong) {
// 			    return song->toFile() == targetSong->getName();
// 		    });
// 		if (found == target.end()) {
// 			destination.emplace_back(song);
// 		} else {
// 			pred(song, *found);
// 		}
// 	}
// }

// TODO: create library class and foreach
void Program::organizeSongs() {
	for (auto &album : mLibrary) {
		auto found =
		    std::find_if(mDownloaded.begin(), mDownloaded.end(),
		                 [&](auto &downAlbum) -> bool {
			                 return downAlbum.second->name == album.first;
		                 });

		if (found != mDownloaded.end()) {
			auto downloadedAlbum = found->second;
			for (auto &libSong : album.second->songs) {
				auto foundSong = std::find_if(
				    downloadedAlbum->songs.begin(),
				    downloadedAlbum->songs.end(),
				    [&libSong](Song::Ptr song) -> bool {
					    return song->toFile() == libSong->getName();
				    });
				if (foundSong == downloadedAlbum->songs.end()) {
					mSongsTodelete.push_back(libSong);
				}
			}

			// for (auto &downSong : found->second->songs) {
			// 	bool isFound = false;
			// 	for (auto &libSong : libAlbum->songs) {
			// 		auto startOfName =
			// downSong->getName().find_first_of('-') + 2; // + 2 to
			// accommodate for hyphon and
			// 		                      // space after it
			// 		auto size = downSong->getName().size();
			// 		// TODO: check if downSongName is ok
			// 		auto downSongName = downSong->getName().substr(
			// 		    startOfName + 2, size - startOfName);

			// 		if (libSong->getName() == downSongName) {
			// 			libSong->setStatus(Song::Status::InBoth);
			// 			isFound = true;
			// 		}
			// 		if (libSong->getURL() != downSong->getURL()) {
			// 			libSong->setAlternateUrl(downSong->getURL());
			// 			mUrlToChange.push_back(libSong);
			// 		}
			// 	}

			// 	// If song was not found in library, push to songsToDelete
			// 	if (!isFound) {
			// 		auto startOfName =
			// 		    downSong->getName().find_first_of('-') + 2;
			// 		auto size = downSong->getName().size();
			// 		auto name = downSong->getName().substr(
			// 		    startOfName + 2,
			// 		    size - startOfName - 4); // - 4 is to remove .mp3

			// 		auto song =
			// 		    std::make_shared<Song>(name, downSong->getAlbum());
			// 		song->setStatus(Song::Downloaded);
			// 		mSongsTodelete.push_back(song);

			// 		mLibrary[song->getAlbum()->name].songs.push_back(song);
			// 	}
			// }
		}
	}
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

// void Program::cleanLibrary() {
// 	for (auto &album : mLibrary) {
// 		std::erase_if(album.songs, [](Song::Ptr &song) -> bool {
// 			return song->getStatus() == Song::Status::InBoth;
// 		});
// 	}

// 	// pushing downloaded songs and urlToChange
// 	size_t downloaded = 0;
// 	for (auto &[albumName, album] : mLibrary) {
// 		for (auto &song : album->songs) {
// 			if (song->getStatus() == Song::Downloaded)
// 				mSongsTodelete.push_back(song);
// 			else if (song->getStatus() == Song::DifferentUrl)
// 				mUrlToChange.push_back(song);
// 			else if (song->getStatus() == Song::Library)
// 				mSongsToDownload.push_back(song);
// 		}
// 	}

// 	changeUrls();

// 	deleteUnwantedSongs();

// 	for (auto &album : mLibrary) {
// 		std::erase_if(album.second->songs, [](Song::Ptr &song) -> bool {
// 			return song->getStatus() == Song::Status::Downloaded;
// 		});
// 	}
// }

std::optional<std::string> getName(const std::string &line) {
	if (line.length() == 0 || !line.starts_with('['))
		return {};
	int i = 1;
	while (line[i] != ']' && uint(i) < line.length())
		i++;
	return std::string(line.begin() + 1, line.begin() + i);
}

std::optional<std::string> getThumbnail(const std::string &line) {
	if (line.length() == 0 || !line.starts_with("## ["))
		return {};
	int i = 1;
	while (line[i] != ']' && uint(i) < line.length())
		i++;
	while (line[i] != '(' && uint(i) < line.length())
		i++;
	int j = i;
	while (line[j] != ')' && uint(i) < line.length())
		j++;

	return std::string(line.begin() + i + 1, line.begin() + j);
}

std::optional<std::string> getLink(const std::string &line) {
	if (line.length() == 0 || !line.starts_with('['))
		return {};
	int i = 1;
	while (line[i] != ']' && uint(i) < line.length())
		i++;
	while (line[i] != '(' && uint(i) < line.length())
		i++;
	int j = i;
	while (line[j] != ')' && uint(i) < line.length())
		j++;

	return std::string(line.begin() + i + 1, line.begin() + j);
}

std::optional<std::string> getAlbum(const std::string &line) {
	if (line.length() == 0 || !line.starts_with("## "))
		return {};

	int i = 4;
	while (line[i] != ']' && uint(i) < line.length())
		i++;
	return std::string(line.begin() + 4, line.begin() + i);
}

std::optional<std::string> getArtist(const std::string &line) {
	if (line.length() == 0 || !line.starts_with("# "))
		return {};

	return line.substr(2, line.size() - 1);
}
