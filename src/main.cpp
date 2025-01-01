#include <getters.hpp>
#include <vector>
#include <song.hpp>
#include <fstream>
#include <iostream>
/* #include <iomanip> */
#include <json.hpp>

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
	/* Song s("what"); */
	/* s.metadata.year = "1986"; */
	/* s.metadata.artist = "Metallica"; */
	/* s.metadata.album = "Master of Puppets"; */
	/* s.metadata.imageUrl = "https://i.scdn.co/image/ab67616d0000b2731a84d71391df7469c5ab8539"; */
	/* s.URL = "https://www.youtube.com/watch?v=E0ozmU9cJDg"; */
	/* s.download("/home/tal/Music/t"); */
}

void testingunneeded() {
	std::ifstream libFile("/home/tal/Documents/Obsidian Vault/music/music.md");
	fs::path musicPath = "/home/tal/Music";
	/* std::vector<Song::Ptr> downloaded = getDownloaded(musicPath); */
	/* std::cout << getName("[Battery](https://www.youtube.com/watch?v=vA1nlwTbCvg&list=OLAK5uy_lHtcjdo7MU4A3Tt3MuErEdEOOqs4UgngE&index=1)").value_or("no value") << "\n"; */

	auto library = getLibrary(libFile);

	for(auto& album: library) {
		for(auto& song: album->songs) {
			std::cout << song->trackNumber << ": " << album->name << " - " << song->name << " | " << album->artist << " | " << album->year << "\n";
		}
		std::cout << std::endl;
	}

	/* deleteUnneededSongs(downloaded, library, musicPath); */

}

int main() {
	testingunneeded();
    return 0;
}
