#include "https.hpp"
#include <album.hpp>
#include <exec.hpp>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <song.hpp>
#include <utils.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

void run(const fs::path &musicPath, const std::string &libPath) {
	std::ifstream libFile(libPath);
	auto downloaded = getDownloaded(musicPath);
	auto library = getLibrary(libFile);

	organizeSongs(library, downloaded);
	cleanLibrary(library, musicPath);

	uint cleanLibraries = 0;
	uint total = 0;

	for (auto &[albumName, album] : library) {
		total++;
		if (album->songs.size() == 0) {
			cleanLibraries++;
			continue;
		}
		album->download(musicPath);
	}

	if (cleanLibraries == total) {
		std::cout << "No songs to download! :D\n";
	}
}

typedef std::unordered_map<std::string, Album::Ptr> libType;

std::ostream &operator<<(std::ostream &stream, const libType &lib) {
	for (auto &[albumName, album] : lib) {
		stream << albumName << std::endl;
		for (auto &song : album->songs) {
			stream << song->name << " | " << song->status << std::endl;
		}
		stream << std::endl;
	}
	return stream;
}

void testLyrics() {
	std::ifstream file("api.env");
	std::string apiKey;
	std::getline(file, apiKey);
	apiKey = apiKey.substr(apiKey.find(" = ") + 3);
	auto res = getLyrics("Metallica", "Master Of Puppets", apiKey);
	std::cout << res.value_or("Error") << std::endl;
}

// TODO: Add a list at the end of the downloading showing if there were any
// errors downloading
#ifndef NDEBUG
int main() {
	run("/home/tal/Music/M", "/home/tal/Documents/notes/music/musicTemp.md");
	return 0;
}
#else
int main(int argc, char **argv) {
	if (argc != 3) {
		std::cout
		    << "Usage: " << argv[0]
		    << " {Path to music folder} {Path to music library(.md file)}\n";
		return 1;
	}
	run(argv[1], argv[2]);
	return 0;
}
#endif
