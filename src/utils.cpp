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
collectToDelete(const std::unordered_map<std::string, Album::Ptr> &library,
                const std::unordered_map<std::string, Album::Ptr> &downloaded) {
	std::list<Song::Ptr> list;
	for (auto &[downloadAlbumName, downloadAlbum] : downloaded) {
		auto libraryAtCurrentDownloaded = library.find(downloadAlbumName);
		if (libraryAtCurrentDownloaded == nullptr) {
			for (const auto &song : downloadAlbum->songs) {
				list.push_back(song);
			}
			continue;
		}

		const auto &libraryAlbum = library.at(downloadAlbumName);
		auto &libSongs = libraryAlbum->songs;
		for (auto &downloadedSong : downloadAlbum->songs) {
			auto found =
			    std::find_if(libSongs.begin(), libSongs.end(),
			                 [&](const Song::Ptr &libSong) {
				                 return (libSong->toFile() ==
				                         androidify(downloadedSong->getName()));
			                 });
			if (found == libSongs.end()) {
				list.push_back(downloadedSong);
			}
		}
	}
	return list;
}

std::vector<Song::Ptr>
collectUrls(const std::unordered_map<std::string, Album::Ptr> &library,
            const std::unordered_map<std::string, Album::Ptr> &downloaded) {
	std::vector<Song::Ptr> list;
	for (auto &[downloadAlbumName, downloadAlbum] : downloaded) {
		std::cout << library.at(downloadAlbumName) << "\n";
		if (library.at(downloadAlbumName).get() != nullptr) {
			for (const auto &song : downloadAlbum->songs) {
				list.push_back(song);
			}
			continue;
		}

		const auto &libraryAlbum = library.at(downloadAlbumName);
		const auto &libSongs = libraryAlbum->songs;
		for (auto &downloadedSong : downloadAlbum->songs) {
			auto found =
			    std::find_if(libSongs.begin(), libSongs.end(),
			                 [&](const Song::Ptr &libSong) {
				                 return (libSong->getURL() !=
				                         androidify(downloadedSong->getURL()));
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
    const std::unordered_map<std::string, Album::Ptr> &downloaded) {
	std::list<Song::Ptr> list;

	for (auto &[libraryAlbumName, libraryAlbum] : library) {
		if (libraryAlbum.get() == nullptr)
			continue;
		auto downloadedCurrentLibrary = downloaded.find(libraryAlbumName);
		if (downloadedCurrentLibrary == nullptr) {
			for (auto &librarySong : libraryAlbum->songs) {
				list.push_back(librarySong);
			}
			continue;
		}
		const auto &downloadedAlbum = downloaded.at(libraryAlbumName);
		auto &downloadedSongs = downloadedAlbum->songs;
		for (auto &librarySong : libraryAlbum->songs) {
			auto found =
			    std::find_if(downloadedSongs.begin(), downloadedSongs.end(),
			                 [&](const Song::Ptr &downloadedSong) {
				                 return librarySong->toFile() ==
				                        androidify(downloadedSong->getName());
			                 });
			if (found == downloadedSongs.end()) {
				list.push_back(librarySong);
			}
		}
	}
	return list;
}

// Collects every song that needs to be deleted
std::map<std::string, Album::Ptr> collectToDownload(
    const std::unordered_map<std::string, Album::Ptr> &library,
    const std::unordered_map<std::string, Album::Ptr> &downloaded) {
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

std::string androidify(const std::string &string) {
	std::string ret = string;
	for (auto &c : ret) {
		switch (c) {
		case '*':
			c = '+';
			break;
		case '?':
		case '/':
			c = '_';
			break;
		default:;
		}
	}
	return ret;
}
