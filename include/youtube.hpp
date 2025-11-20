#pragma once
#include <filesystem>
#include <format>
#include <fstream>
#include <https.hpp>
#include <json.hpp>
#include <map>
#include <string>

class Youtube {
  public:
	Youtube(const std::filesystem::path &libPath, const std::string &link,
	        const std::string &apiKey = "");

	void setApiKey(const std::string &key);

	void getVideos();
	void getTitle();

	void writeToFile();

  private:
	void loadApiKey();

	std::filesystem::path mLibPath;
	std::filesystem::path mConfigFile;
	std::string mPlaylistId;
	std::string mPlaylistTitle;
	std::map<std::string, std::string> mVideosLink;
	std::string mApiKey;
};
