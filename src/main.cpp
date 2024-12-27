#include <iostream>
#include <json.hpp>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <cassert>
#include <vector>
#include <song.hpp>
#include <filesystem>

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
	if(line.length() == 0 || line.find_first_of("##") != 0)
		return {};

	return std::string(line.begin() + 3, line.end());
}

std::vector<Song::Ptr> getLibrary(std::ifstream& inFile) {
	std::string line;
	std::vector<Song::Ptr> songs;
	while(std::getline(inFile, line)) {
		Song::Ptr song = std::make_unique<Song>();
		if(auto name = getName(line))
			song->name = name.value();

		if(song->name.length() > 0)
			songs.emplace_back(std::move(song));
	}
	return songs;
}

std::vector<Song::Ptr> getDownloaded(const fs::path& path) {
	std::vector<Song::Ptr> songs;
	for(const auto& pathIt: fs::directory_iterator(path)) {
		auto lastSlash = pathIt.path().string().find_last_of('/');
		auto pathstr = pathIt.path().string();
		Song::Ptr song = std::make_unique<Song>(std::string(pathstr.begin() + lastSlash + 1, pathstr.end()));

		songs.emplace_back(std::move(song));
	}

	return songs;
}

void cleanDuplicateSongs(std::vector<Song::Ptr>& songs) {
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

void test() {
	std::vector<Song::Ptr> songs;
	auto a = std::make_unique<Song>("asd");
	songs.emplace_back(std::move(a));

	auto b = std::make_unique<Song>("asd.mp3");
	songs.emplace_back(std::move(b));

	auto c = std::make_unique<Song>("qwe.mp3");
	songs.emplace_back(std::move(c));

	auto d = std::make_unique<Song>("qwe");
	songs.emplace_back(std::move(d));

	auto e = std::make_unique<Song>("zxc");
	songs.emplace_back(std::move(e));

	auto f = std::make_unique<Song>("zxc.mp3");
	songs.emplace_back(std::move(f));

	auto g = std::make_unique<Song>("asdasdasd");
	songs.emplace_back(std::move(g));

	auto h = std::make_unique<Song>("dsadsadsd.mp3");
	songs.emplace_back(std::move(h));

	cleanDuplicateSongs(songs);
	int i = 0;
	for(auto& song: songs) {
		i++;
		std::cout << i << ": " << *song << "\n";
	}
}

void notTest() {
	std::ifstream libFile("/home/tal/Documents/Obsidian Vault/music/music.md");
	std::vector<Song::Ptr> songs;

	auto downloaded = getDownloaded("/home/tal/Music");
	for(auto& a: downloaded) {
		songs.emplace_back(std::move(a));
	}

	auto lib = getLibrary(libFile);
	for(auto& a: lib) {
		songs.emplace_back(std::move(a));
	}

	cleanDuplicateSongs(songs);
	int i = 0;
	for(auto& song: songs) {
		i++;
		std::cout << i << ": " << *song << "\n";
	}
}

int main() {
	notTest();

    return 0;
}
