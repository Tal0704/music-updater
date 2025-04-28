#pragma once
#include <string>
#include <memory>
#include <ostream>
#include <filesystem>

struct Album;

struct Song 
{
	typedef std::unique_ptr<Song> Ptr;

	enum Status {
		InBoth,
		Downloaded,
		Library,
	};

	Song(const Album* album, Status status = Status::InBoth);
	Song(const std::string& name, const Album* album, Status status = Status::InBoth);

	void download(const std::filesystem::path& path);
	bool isFile();

	// TODO: Change to shared_ptr so all of the songs in the album could point to the same album
	const Album* album;
    std::string name;
    std::string URL;
	int trackNumber;
	Status status;
};

std::ostream& operator << (std::ostream& stream, const Song::Status& status);
