#include <helpers.hpp>
#include <song.hpp>
#include <fstream>
#include <json.hpp>
#include <iostream>

namespace fs = std::filesystem;
using json = nlohmann::json;

void testingunneeded() {
	std::ifstream libFile("/home/tal/Documents/Obsidian Vault/music/music.md");
	fs::path musicPath = "/home/tal/Music";
	auto downloaded = getDownloaded(musicPath);
	auto library = getLibrary(libFile);

	cleanLibrary(downloaded, library);
	deleteUnneeded(downloaded, library, musicPath);

}

void testingPopulateAlbumMetadata() {
	std::ifstream libFile("/home/tal/Documents/Obsidian Vault/music/music.md");
	auto library = getLibrary(libFile);

	for(auto& album: library) {
		album->populateMetadata();
		std::cout << album->name << " | " << album->year << " | " << album->imageURL <<" | " << album->artist << "\n";
	}
}

void run() {
	std::ifstream libFile("/home/tal/Documents/notes/music/music.md");
	fs::path musicPath = "/home/tal/Desktop/songs";
	auto downloaded = getDownloaded(musicPath);
	auto library = getLibrary(libFile);

	cleanLibrary(downloaded, library);
	deleteUnneeded(downloaded, library, musicPath);

	uint cleanLibraries = 0;

	for(auto& album: library) {
		if(album->songs.size() == 0) {
			cleanLibraries++;
			continue;
		}
		album->populateMetadata();
		album->download(musicPath);
	}

	if(cleanLibraries == library.size()) {
		std::cout << "No songs to download!\n";
	}
}

void test() {

}

int main() {
	run();
	return 0;
}
