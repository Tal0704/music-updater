#include <getters.hpp>
#include <vector>
#include <song.hpp>
#include <iostream>
#include <fstream>

void test() {
	Song s("what!");
	s.URL = "https://www.youtube.com/watch?v=E0ozmU9cJDg";
	s.download("/home/tal/Music");
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
	test();
    return 0;
}
