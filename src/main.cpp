#include <helpers.hpp>
#include <song.hpp>
#include <fstream>
#include <json.hpp>
#include <iostream>

namespace fs = std::filesystem;
using json = nlohmann::json;

/* std::ostream& operator<<(std::ostream& stream, const std::vector<Song::Ptr>& songs) { */
/* 	std::cout << "Track | Song | Album\t | Artist | Year" << "\n"; */
/* 	for(auto& song: songs) { */
/* 		std::cout << std::setw(8) << song->metadata.trackNumber; */
/* 		std::cout << std::setw(7) << song->name; */
/* 		std::cout << std::setw(13) << song->metadata.album; */
/* 		std::cout << std::setw(10) << song->metadata.artist; */
/* 		std::cout << std::setw(4) << song->metadata.year; */
/* 		std::cout << "\n"; */
/* 	} */
/* 	return stream; */
/* } */

void testingDownload() {
	Album::Ptr a = std::make_unique<Album>();
	Song s("orion", a);
	s.album->year = "1986";
	s.album->artist = "Metallica";
	s.album->name = "Master of Puppets";
	s.album->imageURL = "https://i.scdn.co/image/ab67616d0000b2731a84d71391df7469c5ab8539";
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

	for(auto& album: library) {
		album->populateMetadata();
		std::cout << album->name << " | " << album->year << " | " << album->imageURL <<" | " << album->artist << "\n";
	}
}

int main() {
	std::ifstream libFile("/home/tal/Documents/Obsidian Vault/music/music.md");
	fs::path musicPath = "/home/tal/Music";
	auto downloaded = getDownloaded(musicPath);
	auto library = getLibrary(libFile);

	cleanLibrary(downloaded, library);
	deleteUnneeded(downloaded, library, musicPath);

	uint cleanLibraries = 0;

	for(auto& album: library) {
		if(album->songs.size() != 0) {
			cleanLibraries++;
			continue;
		}
		album->populateMetadata();
		album->download(musicPath);
		/* std::cout << album->name << " | " << album->year << " | " << album->imageURL <<" | " << album->artist << "\n"; */
	}

	if(cleanLibraries == library.size()) {
		std::cout << "No songs to download!\n";
	}

    return 0;
}
