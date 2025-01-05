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

std::optional<std::string> getLink(const std::string& line) {
	if(line.length() == 0 || line[0] != '[')
		return {};
	int i = 1;
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

	return std::string(line.begin() + 3, line.end());
}

std::vector<Album::Ptr> getLibrary(std::ifstream& inFile) {
	std::string line;
	std::vector<Album::Ptr> albums;
	while(std::getline(inFile, line)) {
		Album::Ptr album = std::make_unique<Album>();
		auto albumName = getAlbum(line);
		if(!albumName.has_value()) {
			continue;
		}
		album->name = albumName.value();

		int i = 1;
		while(line != "") {
			if(!std::getline(inFile, line)) {
				break;
			}
			Song::Ptr song = std::make_unique<Song>(album);
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
		Song::Ptr song = std::make_unique<Song>(placeHolder);
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
	for(auto& album: library) {
		for(auto& song: album->songs)
			songs.emplace_back(song.get());
	}
	for(auto& downloadedSong: downloaded) {
		auto found = std::find_if(songs.begin(), songs.end(), [&](const Song* song) -> bool {
					return song->name == downloadedSong->name.substr(0, downloadedSong->name.size() - 4);
				});
		if(found == songs.end()) {
			fs::remove(path.string() + "/" + downloadedSong->name);
		}
	}
}

void populateAlbum(Album::Ptr& albumToPopulate) { 
	auto& song = albumToPopulate->songs[0];
	json data = json::parse(std::ifstream("data.json"));
	for(auto& album: data["albums"]["items"]) {
		if(album["album_type"].dump() == "album" && album["total_tracks"] == albumToPopulate->songs.size()) {
			song->album->imageURL = album["images"][0].dump();
			song->album->year = album["release_data"].dump();
			song->album->year = song->album->year.substr(0, 4);
			song->album->artist = album["artists"][0]["name"].dump();
		}
	}
}

