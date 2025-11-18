#pragma once
#include <album.hpp>
#include <filesystem>
#include <library.hpp>
#include <list>
#include <song.hpp>
#include <string>

class Program {
  public:
	Program(const std::filesystem::path &musicPath, const std::string &library);
	~Program();
	void run();

  private:
	void loadLibrary();
	void loadDownloaded();

	void organizeSongs();
	void cleanLibrary();

	void changeUrls();
	void deleteUnwantedSongs();
	void download();
	void clean();

	std::filesystem::path mMusicPath;
	std::ifstream mLibraryFile;

	// std::unordered_map<std::string, Album::Ptr> mLibrary;
	// std::unordered_map<std::string, Album::Ptr> mDownloaded;

	Library mLibrary;
	Library mDownloaded;

	Library mAlbumsToDownload;
	std::list<Song::Ptr> mSongsTodelete;
	std::vector<Song::Ptr> mUrlToChange;
};
