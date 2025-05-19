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

	for(auto& [albumName, album]: library) {
		if(album->songs.size() == 0) {
			cleanLibraries++;
			continue;
		}
		album->populateMetadata();
		album->download(musicPath);
	}

	if(cleanLibraries == library.size()) {
		std::cout << "No songs to download! :D\n";
	}
}

void print(std::unordered_map<std::string, Album::Ptr>& library) {
	std::ofstream file("test.txt");

	for(auto& [albumName, album]: library) {
		if(!album->songs.empty())
		{
			file << albumName << "\n";
			for(auto& song: album->songs) {
				file << song->name << " | " << song->status << "\n";
			}
			file << std::endl;
		}
	}
}

void testOrganize(const fs::path& musicPath, const std::string& libPath) {
	std::ifstream libFile(libPath);
	auto downloaded = getDownloaded(musicPath);
	auto library = getLibrary(libFile);

	organizeSongs(library, downloaded);
	print(library);
	return;
	cleanLibrary(library, musicPath);


	return;


	uint cleanLibraries = 0;

	for(auto& [albumName, album]: library) {
		// if(album->songs.size() == 0) {
		// 	cleanLibraries++;
		// 	continue;
		// }
		album->populateMetadata();
		album->download(musicPath);
	}

	if(library.size() == 0) {
		std::cout << "No songs to download! :D\n";
	}
}

#ifndef NDEBUG
int main() {
	testOrganize("/home/tal/Music/temp", "/home/tal/Documents/notes/music/musicTemp.md");
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
