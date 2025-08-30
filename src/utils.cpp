#include <algorithm>
#include <iostream>
#include <utils.hpp>

bool confirmUserInput(const std::string &message) {
	if (!message.empty())
		std::cout << message << "\n";
	std::cout << "y/N\n";

	std::string answer;
	std::getline(std::cin, answer);

	return (answer == "y" || answer == "Y");
}

// Collects every song that needs to be deleted
std::list<Song::Ptr>
collectToDelete(std::unordered_map<std::string, Album::Ptr> &library,
                const std::unordered_map<std::string, Album::Ptr> &downloaded) {
	std::list<Song::Ptr> list;
	for (auto &[downloadAlbumName, downloadAlbum] : downloaded) {
		if (library[downloadAlbumName].get() == nullptr) {
			for (const auto &song : downloadAlbum->songs) {
				list.push_back(song);
			}
			continue;
		}
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

std::list<Song::Ptr> collectSongsToDownload(
    const std::unordered_map<std::string, Album::Ptr> &library,
    std::unordered_map<std::string, Album::Ptr> &downloaded) {
	std::list<Song::Ptr> list;

	for (auto &[libraryAlbumName, libraryAlbum] : library) {
		if (libraryAlbum.get() == nullptr)
			continue;
		if (downloaded[libraryAlbumName].get() == nullptr) {
			for (auto &librarySong : libraryAlbum->songs) {
				std::cout << librarySong->getAlbum()->artist << "\n";
				list.push_back(librarySong);
			}
			continue;
		}
		const auto &downloadedAlbum = downloaded.at(libraryAlbumName);
		auto &downloadedSongs = downloadedAlbum->songs;
		for (auto &librarySong : libraryAlbum->songs) {
			auto found = std::find_if(
			    downloadedSongs.begin(), downloadedSongs.end(),
			    [&](const Song::Ptr &downloadedSong) {
				    return librarySong->toFile() == downloadedSong->getName();
			    });
			if (found == downloadedSongs.end()) {
				list.push_back(librarySong);
			}
		}
	}
	return list;
}

// Collects every song that needs to be deleted
std::map<std::string, Album::Ptr>
collectToDownload(const std::unordered_map<std::string, Album::Ptr> &library,
                  std::unordered_map<std::string, Album::Ptr> &downloaded) {
	std::map<std::string, Album::Ptr> map;
	auto songs = collectSongsToDownload(library, downloaded);

	for (auto &song : songs) {
		if (map[song->getAlbum()->name].get() == nullptr) {
			map[song->getAlbum()->name] =
			    std::make_shared<Album>(song->getAlbum()->name);
			map[song->getAlbum()->name]->imageURL = song->getAlbum()->imageURL;
		}
		map[song->getAlbum()->name]->songs.push_back(song);
	}
	return map;
}

// Getters

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
