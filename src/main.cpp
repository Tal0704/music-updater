#include <getters.hpp>
#include <vector>
#include <song.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

std::ostream& operator<<(std::ostream& stream, const std::vector<Song::Ptr>& songs) {
	std::cout << "Track | Song | Album\t | Artist | Year" << "\n";
	for(auto& song: songs) {
		std::cout << std::setw(8) << song->metadata.trackNumber;
		std::cout << std::setw(7) << song->name;
		std::cout << std::setw(13) << song->metadata.album;
		std::cout << std::setw(10) << song->metadata.artist;
		std::cout << std::setw(4) << song->metadata.year;
		std::cout << "\n";
	}
	return stream;
}

void testingDownload() {
	Song s("what");
	s.metadata.year = "1986";
	s.metadata.artist = "Metallica";
	s.metadata.album = "Master of Puppets";
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
		populateSong(song);
	}
	/* std::cout << library << "\n"; */
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
