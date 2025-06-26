#pragma once
#include <fileMetadata.hpp>
#include <filesystem>
#include <memory>
#include <ostream>
#include <string>

struct Album;

struct Song {
  public:
	typedef std::unique_ptr<Song> Ptr;

	enum Status {
		InBoth,
		DifferentUrl,
		Downloaded,
		Library,
	};

	Song(const std::string &name, std::shared_ptr<Album> album,
	     Status status = Status::InBoth);

	void download(const std::filesystem::path &path);

	std::shared_ptr<Album> album;
	std::string URL;
	std::string alternateUrl;
	int trackNumber;
	Status status;
	std::string name;
	std::string lyrics;
};

bool operator==(const Song &left, const Song &right);

std::ostream &operator<<(std::ostream &stream, const Song::Status &status);
