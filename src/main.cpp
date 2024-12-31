#include <getters.hpp>
#include <vector>
#include <song.hpp>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

void testingDownload() {
	Song s("what");
	s.metadata.year = "1986";
	s.metadata.artist = "Metallica";
	s.metadata.album = "Master of Puppets";
	s.metadata.lyrics = "im pulling your strings!";
	s.metadata.imageUrl = "https://i.scdn.co/image/ab67616d0000b2731a84d71391df7469c5ab8539";
	s.URL = "https://www.youtube.com/watch?v=E0ozmU9cJDg";
	s.download("/home/tal/Music/t");
}

void testingunneeded() {
	std::ifstream libFile("/home/tal/Documents/Obsidian Vault/music/music.md");
	fs::path musicPath = "/home/tal/Music";
	std::vector<Song::Ptr> downloaded = getDownloaded(musicPath);
	std::vector<Song::Ptr> library = getLibrary(libFile);

	deleteUnneededSongs(downloaded, library, musicPath);

	for(auto& song: library) {
		std::cout << song->metadata.trackNumber << ": " << song->name << " | " << song->metadata.album << "\n";
	}
}

void notTest() {
	std::ifstream libFile("/home/tal/Documents/Obsidian Vault/music/music.md");
	fs::path musicPath = "/home/tal/Music";
	std::vector<Song::Ptr> songs;


	auto downloaded = getDownloaded(musicPath);
	for(auto& a: downloaded) {
		songs.emplace_back(std::move(a));
	}

	auto lib = getLibrary(libFile);
	for(auto& a: lib) {
		songs.emplace_back(std::move(a));
	}

	for(auto& song: songs) {
	}
}

int main() {
	testingunneeded();
    return 0;
}
