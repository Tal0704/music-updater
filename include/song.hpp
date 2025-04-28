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

	const Album* album;
	Song(const Album* album, Status status = Status::InBoth);
	Song(const std::string& name, const Album* album, Status status = Status::InBoth);

	void download(const std::filesystem::path& path);
	bool isFile();

    std::string name;
    std::string URL;
	int trackNumber;
	Status status;
};

std::ostream& operator << (std::ostream& stream, const Song& song);
