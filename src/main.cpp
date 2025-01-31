#include <helpers.hpp>
#include <exec.hpp>
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
		album->populateMetadata(musicPath);
		album->download(musicPath);
	}

	if(cleanLibraries == library.size()) {
		std::cout << "No songs to download!\n";
	}
}

void test() {
	std::string getImageData = "curl -L --request GET --url https://coverartarchive.org/release/264b958d-e9fd-4cde-9759-e9d40df12a94/front --output";
	auto res = exec(getImageData);
	std::ofstream file;
	file.open("a.jpg");
	file << res;
}

int main() {
	run();
	return 0;
}
