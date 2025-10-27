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
// TODO: try to use lib iterator
std::list<Song::Ptr> collectToDelete(Library &library, Library &downloaded) {
	std::list<Song::Ptr> list;

	for (auto &downloadedSong : downloaded) {
		auto found = std::find_if(library.begin(), library.end(),
		                          [&](const Song::Ptr &libSong) -> bool {
			                          return downloadedSong->getName() ==
			                                 libSong->getName();
		                          });
		if (found == library.end()) {
			list.emplace_back(downloadedSong);
		}
	}
	// for (auto &[downloadAlbumName, downloadAlbum] : downloaded) {
	// 	auto libraryAtCurrentDownloaded = library.find(downloadAlbumName);
	// 	if (libraryAtCurrentDownloaded == nullptr) {
	// 		for (const auto &song : downloadAlbum->songs) {
	// 			list.push_back(song);
	// 		}
	// 		continue;
	// 	}

	// 	const auto &libraryAlbum = library.at(downloadAlbumName);
	// 	auto &libSongs = libraryAlbum->songs;
	// 	for (auto &downloadedSong : downloadAlbum->songs) {
	// 		auto found =
	// 		    std::find_if(libSongs.begin(), libSongs.end(),
	// 		                 [&](const Song::Ptr &libSong) {
	// 			                 return (libSong->toFile() ==
	// 			                         androidify(downloadedSong->getName()));
	// 		                 });
	// 		if (found == libSongs.end()) {
	// 			list.push_back(downloadedSong);
	// 		}
	// 	}
	// }
	return list;
}

std::vector<Song::Ptr> collectUrls(Library &library, Library &downloaded) {
	// std::vector<Song::Ptr> list;
	// for (const auto &downloadedAlbum : downloaded) {
	// 	auto libraryAlbum = library.find(downloadedAlbumName);
	// 	if (libraryAlbum != library.end()) {
	// 		for (const auto &libSong : libraryAlbum->second->songs) {
	// 			const auto &downloadedSongs = downloadedAlbum->songs;
	// 			auto downloadedSong = std::find_if(
	// 			    downloadedSongs.begin(), downloadedSongs.end(),
	// 			    [&](const Song::Ptr &downloadedSong) -> bool {
	// 				    return downloadedSong->getName() == libSong->getName();
	// 			    });
	// 			if (downloadedSong == downloadedSongs.end())
	// 				continue;
	// 			if (downloadedSong->get()->getURL() != libSong->getURL()) {
	// 				downloadedSong->get()->setURL(libSong->getURL());
	// 				list.emplace_back(*downloadedSong);
	// 			}
	// 		}
	// 	}
	// }

	// return list;
}

std::list<Song::Ptr> collectToDownload(Library &library, Library &downloaded) {
	std::list<Song::Ptr> list;

	for (const auto &libSong : library) {
		auto found = std::find_if(downloaded.begin(), downloaded.end(),
		                          [&](const Song::Ptr &downloadedSong) {
			                          return libSong->getName() ==
			                                 downloadedSong->getName();
		                          });
		if (found == downloaded.end()) {
			list.emplace_back(libSong);
		}
	}

	return list;
	// for (auto &libraryAlbum : library) {
	// 	if (libraryAlbum.get() == nullptr)
	// 		continue;
	// 	auto downloadedCurrentLibrary = downloaded.find(libraryAlbumName);
	// 	if (downloadedCurrentLibrary == downloaded.end()) {
	// 		for (auto &librarySong : libraryAlbum->songs) {
	// 			list.push_back(librarySong);
	// 		}
	// 		continue;
	// 	}

	// 	const auto &downloadedAlbum = downloaded.find(libraryAlbumName);
	// 	if (downloadedAlbum == downloaded.end())
	// 		continue;

	// 	auto &downloadedSongs = downloadedAlbum->second->songs;
	// 	for (auto &librarySong : libraryAlbum->songs) {
	// 		auto found = std::find_if(
	// 		    downloadedSongs.begin(), downloadedSongs.end(),
	// 		    [&](const Song::Ptr &downloadedSong) {
	// 			    return librarySong->getName() == downloadedSong->getName();
	// 		    });
	// 		if (found == downloadedSongs.end()) {
	// 			list.push_back(librarySong);
	// 		}
	// 	}
	// }
}

// Collects every song that needs to be deleted
// std::map<std::string, Album::Ptr> collectToDownload(Library &library,
//                                                     Library &downloaded) {
// 	// std::map<std::string, Album::Ptr> map;

// 	// auto songs = collectSongsToDownload(library, downloaded);

// 	// for (auto &song : songs) {
// 	// 	if (map[song->getAlbum()->name].get() == nullptr) {
// 	// 		map[song->getAlbum()->name] =
// 	// 		    std::make_shared<Album>(song->getAlbum()->name);
// 	// 		map[song->getAlbum()->name]->imageURL = song->getAlbum()->imageURL;
// 	// 	}
// 	// 	map[song->getAlbum()->name]->songs.push_back(song);
// 	// }
// 	// return map;
// }

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
