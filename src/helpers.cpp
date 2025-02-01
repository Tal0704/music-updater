#include <helpers.hpp>
#include <song.hpp>
#include <algorithm>
#include <json.hpp>
#include <filesystem>
#include <string>
#include <iostream>

namespace fs = std::filesystem;
using json = nlohmann::json;

std::optional<std::string> getName(const std::string& line) {
	if(line.length() == 0 || line[0] != '[')
		return {};
	int i = 1;
	while(line[i] != ']' && uint(i) < line.length()) 
		i++;
	return std::string(line.begin() + 1, line.begin() + i);
}

std::optional<std::string> getThumbnail(const std::string& line) {
	if(line.length() == 0 || line.substr(0, 4) != "## [")
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
	if(line.length() == 0 || line[0] != '[')
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
	if(line.length() == 0 || line.substr(0, 2) != "# ")
		return {};

	return line.substr(2, line.size() - 1);
}

std::vector<Album::Ptr> getLibrary(std::ifstream& inFile) {
	std::string line;
	std::string artist;
	std::vector<Album::Ptr> albums;
	while(std::getline(inFile, line)) {
		Album::Ptr album = std::make_unique<Album>();
		auto albumName = getAlbum(line);
		if(auto temp = getArtist(line)) {
			artist = temp.value();
		}
		if(!albumName.has_value()) {
			continue;
		}
		album->name = albumName.value();

		if(auto thumbnail = getThumbnail(line)) {
			album->imageURL = thumbnail.value();
		}

		int i = 1;
		while(line != "") {
			if(!std::getline(inFile, line)) {
				break;
			}

			Song::Ptr song = std::make_unique<Song>(album.get());
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
		if(artist != "") {
			album->artist = artist;
		}
		albums.emplace_back(std::move(album));
	}

	return albums;
}

std::vector<Song::Ptr> getDownloaded(const fs::path& path) {
	std::vector<Song::Ptr> songs;
	Album::Ptr placeHolder;
	
	for(const auto& pathIt: fs::directory_iterator(path)) {
		auto lastSlash = pathIt.path().string().find_last_of('/');
		auto pathstr = pathIt.path().string();
		Song::Ptr song = std::make_unique<Song>(placeHolder.get());
		song->name = std::string(pathstr.begin() + lastSlash + 1, pathstr.end());

		songs.emplace_back(std::move(song));
	}

	return songs;
}

void cleanLibrary(const std::vector<Song::Ptr>& downloaded, std::vector<Album::Ptr>& library) {
	// Loop over the downloaded songs
	for(auto it = downloaded.begin(); it != downloaded.end(); ++it) {
		auto& downloadedSong = *it;
		// Loop over all the albums in the library
		for(auto& album: library) {
			auto& songs = album->songs;
			// For every album find the song that is in library and is already downloaded
			auto found = std::find_if(songs.begin(), songs.end(), [&](Song::Ptr& song) -> bool {
						return song->name == downloadedSong->name.substr(0, downloadedSong->name.size() - 4);
					});
			// If not found any song, delete it. Else erase it from the library
			if(found != songs.end()) {
				songs.erase(found);
				break;
			}
		}
	}
}

void deleteUnneeded(const std::vector<Song::Ptr>& downloaded, std::vector<Album::Ptr>& library, const fs::path& path) {
	std::vector<const Song*> songs;
	std::vector<const Song*> toDelete;
	for(auto& album: library) {
		for(auto& song: album->songs)
			songs.emplace_back(song.get());
	}
	for(auto& downloadedSong: downloaded) {
		if(!downloadedSong->name.ends_with(".mp3"))
			continue;
		auto found = std::find_if(songs.begin(), songs.end(), [&](const Song* song) -> bool {
					return song->name == downloadedSong->name.substr(0, downloadedSong->name.size() - 4);
				});
		if(found != songs.end()) {
			toDelete.emplace_back(downloadedSong.get());
		}
	}

	if(toDelete.size() == 0) {
		std::cout << "No files to delete!\n";
		return;
	}
	std::cout << "Are you sure you want to delete: \n";
	for(auto& song: toDelete) {
		std::cout << song->name << "\n";
	}
	std::cout << "y/N\n";
	auto& a = path;
}
