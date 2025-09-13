#pragma once
#include <album.hpp>
#include <filesystem>
#include <list>
#include <map>
#include <song.hpp>
#include <string>
#include <unordered_map>

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

	std::unordered_map<std::string, Album::Ptr> mLibrary;
	std::unordered_map<std::string, Album::Ptr> mDownloaded;

	std::map<std::string, Album::Ptr> mAlbumsToDownload;
	std::list<Song::Ptr> mSongsTodelete;
	std::list<Song::Ptr> mUrlToChange;
};
