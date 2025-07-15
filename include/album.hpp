#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

struct Song;

struct Album {
	typedef std::shared_ptr<Album> Ptr;
	typedef std::vector<std::shared_ptr<Song>> ContainerType;

	Album();
	Album(Album &&other);
	Album(const std::string &name);
	Album(const Album &other);

	void download(const std::filesystem::path &path);
	void populateMetadata();

	void operator=(const Album &other);

	std::vector<std::shared_ptr<Song>> songs;
	std::string name;
	std::string year;
	std::string imageURL;
	std::string artist;
	std::string genre;
	uint totalSize;
};

std::ostream &operator<<(std::ostream &stream, const Album &album);
