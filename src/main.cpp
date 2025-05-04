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

	for(auto& album: library) {
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

int main(int argc, char** argv) {
	if (argc < 3) {
		std::cout << "Usage: " << argv[0] << " {Path to music folder} {Path to music library(.md file)}\n";
		return 1;
	}
	run(argv[1], argv[2]);
	return 0;
}
