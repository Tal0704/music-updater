#include <getters.hpp>
#include <algorithm>
#include <json.hpp>
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

void cleanLibrary(std::vector<Song::Ptr>& songs) {
	for(auto it = songs.begin(); it != songs.end();) {
		auto& song = *it;
		auto found = std::find_if(songs.begin(), songs.end(), [&](Song::Ptr& s) -> bool {
					if(song->isFile())
					{
						return std::string(song->name.begin(), song->name.end() - 4) == s->name;
					}
					return false;
				});

		if(found != songs.end()) {
			songs.erase(found);
		}
		else {
			++it;
		}
	}
}

void deleteUnneededSongs(std::vector<Song::Ptr>& downloaded, std::vector<Album::Ptr>& library, const fs::path& path) {
	for(auto downloadIt = downloaded.begin(); downloadIt != downloaded.end(); ++downloadIt) {
		auto found = std::find_if(library.begin(), library.end(), [&](Album::Ptr& album) -> bool {
				auto& songs = album.get()->songs;
				auto a = std::any_of(songs.begin(), songs.end(), [&](Song::Ptr& song) -> bool {
							return song->name == downloadIt->get()->name.substr(0, downloadIt->get()->name.size() - 4);
						});
				return a;
				});
		if(found == library.end()) {
			std::cout << "deleted: " << path.string() + "/" + downloadIt->get()->name;
			/* fs::remove(path.string() + "/" + downloadIt->get()->name); */
		}
		else {
			library.erase(found);
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
