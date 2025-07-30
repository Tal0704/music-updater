#pragma once
#include <album.hpp>
#include <filesystem>
#include <song.hpp>
#include <string>
#include <unordered_map>

class Program {
  public:
	Program(const std::filesystem::path &musicPath, const std::string &library);
	void run();

  private:
	void loadLibrary();
	void loadDownloaded();

	void organizeSongs();
	void cleanLibrary();

	void changeUrls();
	void deleteUnwantedSongs();

	void download();

	std::filesystem::path mMusicPath;
	std::ifstream mLibraryFile;

	std::unordered_map<std::string, Album::Ptr> mLibrary;
	std::unordered_map<std::string, Album::Ptr> mDownloaded;

	std::vector<Song::Ptr> mSongsToDownload;
	std::vector<Song::Ptr> mSongsTodelete;
	std::vector<Song::Ptr> mUrlToChange;
};
