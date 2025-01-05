#include <helpers.hpp>
#include <song.hpp>
#include <fstream>
#include <json.hpp>
#include <iostream>

namespace fs = std::filesystem;
using json = nlohmann::json;

void testingDownload() {
	Album::Ptr a = std::make_unique<Album>();
	Song s("orion", a.get());
	/* s.album->year = "1986"; */
	/* s.album->artist = "Metallica"; */
	/* s.album->name = "Master of Puppets"; */
	/* s.album->imageURL = "https://i.scdn.co/image/ab67616d0000b2731a84d71391df7469c5ab8539"; */
	s.URL = "https://www.youtube.com/watch?v=E0ozmU9cJDg";
	s.download("/home/tal/Music/t");
}

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

	std::string bearer;
	std::getline(std::ifstream(".bearer"), bearer);

	for(auto& album: library) {
		album->populateMetadata(bearer.c_str());
		std::cout << album->name << " | " << album->year << " | " << album->imageURL <<" | " << album->artist << "\n";
	}
}

int main() {
	std::ifstream libFile("/home/tal/Documents/Obsidian Vault/music/music.md");
	fs::path musicPath = "/home/tal/Music";
	auto downloaded = getDownloaded(musicPath);
	auto library = getLibrary(libFile);
	std::string bearer;
	std::getline(std::ifstream(".bearer"), bearer);

	cleanLibrary(downloaded, library);
	/* deleteUnneeded(downloaded, library, musicPath); */

	uint cleanLibraries = 0;

	for(auto& album: library) {
		if(album->songs.size() == 0) {
			cleanLibraries++;
			continue;
		}
		album->populateMetadata(bearer.c_str());
		std::cout << album->name << " | " << album->year << " | " << album->imageURL <<" | " << album->artist << "\n";
		album->download(musicPath);
	}

	if(cleanLibraries == library.size()) {
		std::cout << "No songs to download!\n";
	}

    return 0;
}
