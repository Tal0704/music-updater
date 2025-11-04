#include <colors.hpp>
#include <format>
#include <iostream>
#include <json.hpp>
#include <optional>
#include <program.hpp>
#include <utils.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

void Program::run() {
	clean();
	loadLibrary();
	loadDownloaded();
	organizeSongs();
	download();
	std::cout << "library\n";
	for (auto &album : mAlbumsToDownload) {
		std::cout << album->getAlbum()->imageURL << "\n";
	}

	// std::cout << "downloaded\n";
	// for (auto &[albumName, album] : mDownloaded) {
	// 	for (const auto &song : album->songs)
	// 		std::cout << *song << "\n";
	// }
	// std::cout << "to change" << "\n";

	// for (const auto &song : mUrlToChange) {
	// 	std::cout << song->getName() << " - " << song->getURL() << "\n";
	// }
}

Program::Program(const std::filesystem::path &musicPath,
                 const std::string &library)
    : mMusicPath(musicPath), mLibraryFile(library) {}

Program::~Program() { clean(); }

bool sureDifferentUrl(const Song &song, const std::string &originalUrl);

void Program::loadLibrary() {
	std::cout << "Loading library...\n";
	std::string line;
	std::string artist;

	uint lineNumber = 0;

	while (std::getline(mLibraryFile, line)) {
		lineNumber++;
		if (line.starts_with("//")) {
			continue;
		}

		if (auto temp = getArtist(line)) {
			artist = temp.value();
		}

		auto albumName = getAlbum(line);
		if (!albumName.has_value()) {
			continue;
		}

		Album::Ptr album = std::make_shared<Album>(albumName.value());
		album->artist = artist;

		if (auto thumbnail = getThumbnail(line)) {
			album->imageURL = thumbnail.value();
		}

		if (!(!((album->imageURL.length() == 0) ^
		        (album->artist.length() == 0)))) {
			throw std::runtime_error(
			    std::format("Did not found image url or the artist in line {}",
			                lineNumber));
		}

		int i = 1;
		while (line != "") {
			if (!std::getline(mLibraryFile, line) || line.empty()) {
				break;
			}

			lineNumber++;
			if (line.starts_with("//")) {
				continue;
			}

			auto songName = getName(line);
			Song::Ptr song = std::make_unique<Song>(
			    songName.value_or(""), album, Song::Status::Library);
			if (auto URL = getLink(line)) {
				if (URL->empty()) {
					std::cerr
					    << std::format("{} - {} Has no link, skipping over it",
					                   album->name, song->getName())
					    << "\n";
					i++;
					continue;
				}
				song->setURL(URL.value());
			}

			if (song->getName().length() > 0) {
				song->setTrackNumber(i++);
				album->songs.emplace_back(std::move(song));
			}
		}

		album->totalSize = album->songs.size();
		mLibrary.addAlbum(album);
	}
	std::cout << Colors::green << "Finished loading library!\n"
	          << Colors::reset;
}

void Program::loadDownloaded() {
	std::cout << "Loading downloaded songs...\n";
	for (const auto &pathIt : fs::directory_iterator(mMusicPath)) {
		if (!pathIt.is_regular_file()) {
			continue;
		}

		FileMetadata metadata(pathIt.path().c_str());
		auto name = metadata.read("title");
		auto albumName = metadata.read("album");
		auto artistName = metadata.read("artist");
		Album::Ptr album = std::make_unique<Album>(albumName);
		Song::Ptr song =
		    std::make_unique<Song>(name, album, Song::Status::Downloaded);

		try {
			auto songUrl = metadata.read("url");
			song->setURL(songUrl);
		} catch (const char *c) {
			std::cerr << "Couldn't find song url";
		}

		album->artist = artistName;

		// mDownloaded[album->name] = std::move(album);
		if (mDownloaded[albumName].get() == nullptr) {
			mDownloaded[albumName] = std::make_shared<Album>(albumName);
		}
		mDownloaded[albumName]->songs.push_back(std::move(song));
	}
	std::cout << Colors::green << "Finished loading downloaded songs!\n"
	          << Colors::reset;
}

bool sureDifferentUrl(const Song &song, const std::string &originalUrl) {
	std::cout << song.getName()
	          << ": are you sure you want to replace url?\noriginal: "
	          << originalUrl << "\nnew     : " << song.getURL();
	std::cout << "\ny/N\n";

	std::string answer;
	std::getline(std::cin, answer);

	return (answer == "y" || answer == "Y");
}

void Program::organizeSongs() {
	// mSongsTodelete = collectToDelete(mLibrary, mDownloaded);
	mAlbumsToDownload = collectToDownload(mLibrary, mDownloaded);
	// mUrlToChange = collectUrls(mLibrary, mDownloaded);
};

void Program::changeUrls() {
	for (auto &song : mUrlToChange) {
		std::cout << "Are you sure you want to change to new link for " << song
		          << "\nNew link: " << song->getURL();
		if (confirmUserInput()) {
			// TODO: Add song to mAlbumsToDownload
			fs::remove(mMusicPath / song->toFile());
		}
	}
}

void Program::deleteUnwantedSongs() {
	if (mSongsTodelete.empty()) {
		std::cout << Colors::green << "No songs to delete! :D\n"
		          << Colors::reset;
		return;
	}

	std::cout << "Are you sure you want to delete: \n";
	for (auto &song : mSongsTodelete) {
		std::cout << *song << "\n";
	}

	if (confirmUserInput()) {
		for (auto &song : mSongsTodelete) {
			auto file = mMusicPath / song->getName();
			std::cout << "Deleteing: " << file << "\n";
			fs::remove(file);
		}
	}
}

void Program::download() {
	if (mAlbumsToDownload.empty()) {
		std::cout << Colors::green << "No songs to download!" << Colors::reset
		          << std::endl;
		return;
	}

	mAlbumsToDownload.download(mMusicPath);
	std::cout << Colors::green << "Finished downloading all the songs!"
	          << Colors::reset << "\n";
}

void Program::clean() {
	for (const auto &pathIt : fs::directory_iterator(mMusicPath)) {
		if (!pathIt.is_regular_file()) {
			continue;
		}
		auto foundTemp = pathIt.path().string().find("temp");
		if (foundTemp != std::string::npos) {
			fs::remove(pathIt.path());
		}
	}
}
