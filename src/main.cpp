#include <getters.hpp>
#include <vector>
#include <song.hpp>
#include <iostream>
#include <fstream>

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
