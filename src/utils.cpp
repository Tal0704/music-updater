#include <algorithm>
#include <functional>
#include <iostream>
#include <utils.hpp>

void confirmUserInput(std::function<void(void)> callback) {
	std::cout << "y/N\n";

	std::string answer;
	std::getline(std::cin, answer);

	if (answer == "y" || answer == "Y") {
		callback();
	}
}

// Collects every song that needs to be deleted
std::list<Song::Ptr>
collectToDelete(const std::unordered_map<std::string, Album::Ptr> &library,
                const std::unordered_map<std::string, Album::Ptr> &downloaded) {
	std::list<Song::Ptr> list;
	for (auto &[downloadAlbumName, downloadAlbum] : downloaded) {
		const auto &libraryAlbum = library.at(downloadAlbumName);
		auto &libSongs = libraryAlbum->songs;
		for (auto &downloadedSong : downloadAlbum->songs) {
			auto found = std::find_if(libSongs.begin(), libSongs.end(),
			                          [&](const Song::Ptr &libSong) {
				                          return (libSong->toFile() ==
				                                  downloadedSong->getName());
			                          });
			if (found == libSongs.end()) {
				list.push_back(downloadedSong);
			}
		}
	}
	return list;
}

// Collects every song that needs to be deleted
std::list<Song::Ptr> collectToDownload(
    const std::unordered_map<std::string, Album::Ptr> &library,
    const std::unordered_map<std::string, Album::Ptr> &downloaded) {
	std::list<Song::Ptr> list;
	for (auto &[downloadAlbumName, downloadAlbum] : downloaded) {
		const auto &libraryAlbum = library.at(downloadAlbumName);
		auto &libSongs = libraryAlbum->songs;
		for (auto &downloadedSong : downloadAlbum->songs) {
			auto found = std::find_if(libSongs.begin(), libSongs.end(),
			                          [&](const Song::Ptr &libSong) {
				                          return (libSong->toFile() ==
				                                  downloadedSong->getName());
			                          });
			if (found == libSongs.end()) {
				list.push_back(downloadedSong);
			}
		}
	}
	return list;
}
