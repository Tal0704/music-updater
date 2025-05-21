#include <helpers.hpp>
#include <exec.hpp>
#include <song.hpp>
#include <fstream>
#include <json.hpp>
#include <iostream>
#include <album.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

void run(const fs::path& musicPath, const std::string& libPath) {
	std::ifstream libFile(libPath);
	auto downloaded = getDownloaded(musicPath);
	auto library = getLibrary(libFile);

	organizeSongs(library, downloaded);
	cleanLibrary(library, musicPath);

	uint cleanLibraries = 0;
	uint total = 0;

	for(auto& [albumName, album]: library) {
		total++;
		if(album->songs.size() == 0) {
			cleanLibraries++;
			continue;
		}
		album->populateMetadata();
		album->download(musicPath);
	}

	if(cleanLibraries == total) {
		std::cout << "No songs to download! :D\n";
	}
}

typedef std::unordered_map<std::string, Album::Ptr> libType ;

std::ostream& operator<< (std::ostream& stream, const libType& lib) {
	for(auto& [albumName, album]: lib) {
		stream << albumName << std::endl;
		for (auto& song: album->songs) {
			stream << song->name << " | " << song->status << std::endl;
		}
		stream << std::endl;
	}
	return stream;
}

// void print(std::ostream& stream, const libType& lib) {
// 	for(auto& [albumName, album]: lib) {
// 		stream << albumName << std::endl;
// 		for (auto& song: album->songs) {
// 			stream << song->name << " | " << song->status << std::endl;
// 		}
// 		stream << std::endl;
// 	}
// }

void testOrganize(const fs::path& musicPath, const std::string& libPath) {
	std::ifstream libFile(libPath);
	auto downloaded = getDownloaded(musicPath);
	std::ofstream file("test.txt");
	file << "Library:\n" << downloaded << "\n";
	auto library = getLibrary(libFile);

	organizeSongs(library, downloaded);
	cleanLibrary(library, musicPath);

	uint cleanLibraries = 0;
	uint total = 0;

	for(auto& [albumName, album]: library) {
		total++;
		if(album->songs.size() == 0) {
			cleanLibraries++;
			continue;
		}
		album->populateMetadata();
		album->download(musicPath);
	}

	if(cleanLibraries == total) {
		std::cout << "No songs to download! :D\n";
	}
}

#ifndef NDEBUG
int main() {
	testOrganize("/home/tal/Desktop/music", "/home/tal/Documents/notes/music/music.md");
	return 0;
}
#else
int main(int argc, char** argv) {
	if (argc != 3) {
		std::cout << "Usage: " << argv[0] << " {Path to music folder} {Path to music library(.md file)}\n";
		return 1;
	}
	run(argv[1], argv[2]);
	return 0;
}
#endif
