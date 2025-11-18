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
std::list<Song::Ptr> collectToDelete(Library &library, Library &downloaded) {
	std::list<Song::Ptr> list;
	if (downloaded.empty()) {
		return list;
	}

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
	return list;
}

std::vector<Song::Ptr> collectUrls(Library &library, Library &downloaded) {
	std::vector<Song::Ptr> vector;

	if (library.empty()) {
		return vector;
	}

	// TODO: Check if this logic actually works
	for (const auto &libSong : library) {
		auto found = std::find_if(
		    downloaded.begin(), downloaded.end(),
		    [&](const Song::Ptr &downloadedSong) {
			    return (libSong->getName() == downloadedSong->getName()) &&
			           (libSong->getURL() != downloadedSong->getURL());
		    });
		if (found != downloaded.end()) {
			vector.emplace_back(libSong);
		}
	}

	return vector;
}

Library collectToDownload(Library &library, Library &downloaded) {
	Library lib;

	if (downloaded.empty()) {
		return library;
	}

	for (const auto &libSong : library) {
		auto found = std::find_if(downloaded.begin(), downloaded.end(),
		                          [&](const Song::Ptr &downloadedSong) {
			                          return libSong->getName() ==
			                                 downloadedSong->getName();
		                          });
		if (found == downloaded.end()) {
			lib.addSong(*libSong);
		}
	}

	return lib;
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
