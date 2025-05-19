#include <algorithm>
#include <helpers.hpp>
#include <memory>
#include <song.hpp>
#include <json.hpp>
#include <filesystem>
#include <string>
#include <iostream>
#include <cassert>
#include <album.hpp>
#include <unordered_map>
#include <fileMetadata.hpp>

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
	if(line.length() == 0 || !line.starts_with("## ["))
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

	int i = 4;
	while(line[i] != ']' && uint(i) < line.length())
		i++;
	return std::string(line.begin() + 4, line.begin() + i);
}

std::optional<std::string> getArtist(const std::string& line) {
	if(line.length() == 0 || !line.starts_with("# "))
		return {};

	return line.substr(2, line.size() - 1);
}

std::unordered_map<std::string, Album::Ptr> getLibrary(std::ifstream& inFile) {
	std::string line;
	std::string artist;
	std::unordered_map<std::string, Album::Ptr> albums;

	while(std::getline(inFile, line)) {
		if(auto temp = getArtist(line)) {
			artist = temp.value();
		} 
		auto albumName = getAlbum(line);
		if(!albumName.has_value()) {
			continue;
		}
		Album::Ptr album = std::make_shared<Album>(albumName.value());
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

			auto songName = getName(line);
			Song::Ptr song = std::make_unique<Song>(songName.value_or(""), album, Song::Status::Library);
			if(auto URL = getLink(line)) {
				song->URL = URL.value_or("");
			}

			if(song->name.length() > 0) {
				song->trackNumber = i++;
				album->songs.emplace_back(std::move(song));
			}
		}
		album->totalSize = album->songs.size();
		if(albums[album->name].get() == nullptr) {
			albums[album->name] = std::make_shared<Album>(album->name);
		}
		albums[album->name] = std::move(album);
	}

	return albums;
}

std::unordered_map<std::string, Album::Ptr> getDownloaded(const fs::path& path) {
	std::unordered_map<std::string, Album::Ptr> albums;

	for(const auto& pathIt: fs::directory_iterator(path)) {
		FileMetadata metadata(pathIt.path().c_str());
		auto lastSlash = pathIt.path().string().find_last_of('/');
		auto pathstr = pathIt.path().string();
		auto name = std::string(pathstr.begin() + lastSlash + 1, pathstr.end());
		auto albumName = metadata.read("album");
		auto artistName = metadata.read("artist");
		Album::Ptr album = std::make_unique<Album>(albumName);
		Song::Ptr song = std::make_unique<Song>(name, album, Song::Status::Downloaded);
		album->artist = artistName;

		if(albums[albumName].get() == nullptr) {
			albums[albumName] = std::make_shared<Album>(albumName);
		}
		albums[albumName]->songs.push_back(std::move(song));
	}

	return albums;
}

void organizeSongs(std::unordered_map<std::string, Album::Ptr>& library, std::unordered_map<std::string, Album::Ptr>& downloaded) {
	std::vector<Song*> librarySongs;

	for(auto& [libAlbumName, libAlbum]: library) {
		auto found = std::find_if(downloaded.begin(), downloaded.end(), [&](auto& downAlbum) -> bool {
				return downAlbum.second->name == libAlbumName;
				});
		bool isFound = false;
		if (found != downloaded.end()) {
			for(auto& downSong: downloaded[libAlbumName]->songs) {
				for(auto& libSong: libAlbum->songs) {
					auto firstHyphon = downSong->name.find_first_of('-');
					auto size = downSong->name.size();
					auto downSongName = downSong->name.substr(firstHyphon + 2, size - firstHyphon - 6);
					if(androidify(libSong->name) == downSongName) {
						libSong->status = Song::Status::InBoth;
						isFound = true;
					}
				}
				if (!isFound) {
					if(library[downSong->album->name].get() == nullptr) {
						library[downSong->album->name] = std::make_shared<Album>(downSong->album->name);
					}
					auto firstHyphon = downSong->name.find_first_of('-');
					auto size = downSong->name.size();
					downSong->name = downSong->name.substr(firstHyphon + 2, size - firstHyphon - 6);
					const auto& name = downSong->album->name;
					library[name]->songs.push_back(std::move(downSong));
				}
			}
		}
	}
	return;
}

void cleanLibrary(std::unordered_map<std::string, Album::Ptr>& library, const fs::path& path) {
	for(auto& album: library) {
		std::erase_if(album.second->songs, [](Song::Ptr& song) -> bool {
				return song->status == Song::Status::InBoth;
				});
	}

	size_t size = 0;
	for(auto& [albumName, album]: library) {
		// if(album->songs.empty()) {
		// 	library.erase(albumName);
		// }
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
		for(auto& song: album.second->songs)
			if(song->status == Song::Status::Downloaded)
				std::cout << song->album->name << ": " << song->name << "\n";
	}
	std::cout << "y/N\n";

	std::string answer;
	std::getline(std::cin, answer);

	if(answer == "y" || answer == "Y") {
		for(auto& album: library) {
			for(auto& song: album.second->songs) {
				if(song->status == Song::Status::Downloaded) {
					auto toRemove = path.string() + "/" + song->album->artist + " - " + song->name + ".mp3";
					fs::remove(toRemove.c_str());
					std::cout << "Deleteing: " << toRemove << "\n";
				}
			}
		}
	}

	for(auto& album: library) {
		std::erase_if(album.second->songs, [](Song::Ptr& song) -> bool {
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
			case '/':
				c = '_';
				break;
			default:
				;
		}
	}
	return ret;
}
