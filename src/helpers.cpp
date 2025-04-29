#include <helpers.hpp>
#include <memory>
#include <song.hpp>
#include <json.hpp>
#include <filesystem>
#include <string>
#include <iostream>
#include <cassert>
#include <album.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

std::optional<std::string> getName(const std::string& line) {
	if(line.length() == 0 || !line.starts_with('['))
		return {};
	int i = 1;
	while(line[i] != ']' && uint(i) < line.length()) 
		i++;
	return std::string(line.begin() + 1, line.begin() + i);
}

std::optional<std::string> getThumbnail(const std::string& line) {
	if(line.length() == 0 || !line.starts_with("## !["))
		return {};
	int i = 1;
	while(line[i] != ']' && uint(i) < line.length())
		i++;
	while(line[i] != '(' && uint(i) < line.length()) 
		i++;
	int j = i;
	while (line[j] != ')' && uint(i) < line.length())
		j++;

	return std::string(line.begin() + i + 1, line.begin() + j);
}

std::optional<std::string> getLink(const std::string& line) {
	if(line.length() == 0 || !line.starts_with('['))
		return {};
	int i = 1;
	while(line[i] != ']' && uint(i) < line.length())
		i++;
	while(line[i] != '(' && uint(i) < line.length()) 
		i++;
	int j = i;
	while (line[j] != ')' && uint(i) < line.length())
		j++;

	return std::string(line.begin() + i + 1, line.begin() + j);
}

std::optional<std::string> getAlbum(const std::string& line) {
	if(line.length() == 0 || !line.starts_with("## "))
		return {};

	int i = 5;
	while(line[i] != ']' && uint(i) < line.length())
		i++;
	return std::string(line.begin() + 5, line.begin() + i);
}

std::optional<std::string> getArtist(const std::string& line) {
	if(line.length() == 0 || !line.starts_with("# "))
		return {};

	return line.substr(2, line.size() - 1);
}

std::vector<Album::Ptr> getLibrary(std::ifstream& inFile) {
	std::string line;
	std::string artist;
	std::vector<Album::Ptr> albums;

	while(std::getline(inFile, line)) {
		Album::Ptr album = std::make_shared<Album>();
		auto albumName = getAlbum(line);
		if(auto temp = getArtist(line)) {
			artist = temp.value();
		} 
		if(!albumName.has_value()) {
			continue;
		}
		album->name = albumName.value();
		album->artist = artist;

		if(auto thumbnail = getThumbnail(line)) {
			album->imageURL = thumbnail.value();
		}
		assert(!((album->imageURL.length() != 0) ^ (album->artist.length() != 0)));

		int i = 1;
		while(line != "") {
			if(!std::getline(inFile, line)) {
				break;
			}

			Song::Ptr song = std::make_unique<Song>(album, Song::Status::Library);
			auto songName = getName(line);
			song->name = songName.value_or("");
			if(auto URL = getLink(line)) {
				song->URL = URL.value_or("");
			}

			if(song->name.length() > 0) {
				song->trackNumber = i++;
				album->songs.emplace_back(std::move(song));
			}
		}
		album->totalSize = album->songs.size();
		albums.emplace_back(std::move(album));
	}

	return albums;
}

std::vector<Album::Ptr> getDownloaded(const fs::path& path) {
	std::vector<Album::Ptr> albums;

	for(const auto& pathIt: fs::directory_iterator(path)) {
		Album::Ptr album = std::make_unique<Album>("asd");
		auto lastSlash = pathIt.path().string().find_last_of('/');
		auto pathstr = pathIt.path().string();
		Song::Ptr song = std::make_unique<Song>(album, Song::Status::Downloaded);
		song->name = std::string(pathstr.begin() + lastSlash + 1, pathstr.end());

		album->songs.push_back(std::move(song));
		albums.emplace_back(std::move(album));
	}

	return albums;
}

void organizeSongs(std::vector<Album::Ptr>& library, std::vector<Album::Ptr>& downloaded) {
	std::vector<Song*> librarySongs;

	for(auto& album: library) {
		auto& songs = album->songs;
		for(auto& song: songs) {
			librarySongs.push_back(song.get());
		}
	}

	for(auto& down: downloaded) {
		auto& downloadedSong = down->songs[0];
		auto found = false;
		for(auto& song: librarySongs) {
			auto& name = downloadedSong->name;
			if (androidify(song->name) == name.substr(0, name.size() - 4)) {
				found = true;
				song->status = Song::Status::InBoth;
			}
		}
		if(!found) {
			library.push_back(std::move(downloadedSong->album));
		}
	}
}

void cleanLibrary(std::vector<Album::Ptr>& library, const fs::path& path) {
	for(auto& album: library) {
		std::erase_if(album->songs, [](Song::Ptr& song) -> bool {
					return song->status == Song::Status::InBoth;
				});
	}

	size_t size = 0;
	for(auto& album: library) {
		for(auto& song: album->songs) {
			if(song->status == Song::Downloaded)
				size++;
		}
	}

	if (size == 0) {
		std::cout << "No songs to delete! :D\n";
		return;
	}

	std::cout << "Are you sure you want to delete: \n";
	for(auto& album: library) {
		for(auto& song: album->songs)
			if(song->status == Song::Status::Downloaded)
				std::cout << song->name << "\n";
	}
	std::cout << "y/N\n";

	std::string answer;
	std::getline(std::cin, answer);

	if(answer == "y" || answer == "Y") {
		for(auto& album: library) {
			for(auto& song: album->songs) {
				if(song->status == Song::Status::Downloaded) {
					auto toRemove = path.string() + "/" + song->name;
					fs::remove(toRemove.c_str());
					std::cout << "Deleteing: " << toRemove << "\n";
				}
			}
		}
	}

	for(auto& album: library) {
		std::erase_if(album->songs, [](Song::Ptr& song) -> bool {
					return song->status == Song::Status::Downloaded;
				});
	}
}

std::string androidify(const std::string& string) {
	std::string ret = string;
	for(auto& c: ret) {
		switch (c) {
			case '*':
				c = '+';
				break;
			case '?':
				c = '_';
				break;
			default:
				;
		}
	}
	return ret;
}
