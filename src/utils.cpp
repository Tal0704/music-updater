#include <algorithm>
#include <iostream>
#include <utils.hpp>

bool confirmUserInput(const std::string &message) {
	std::cout << message << "\ny/N\n";

	std::string answer;
	std::getline(std::cin, answer);

	return (answer == "y" || answer == "Y");
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
