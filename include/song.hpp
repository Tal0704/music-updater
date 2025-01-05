#pragma once
#include <string>
#include <memory>
#include <ostream>
#include <filesystem>

struct Album;

struct Song 
{
	const Album* album;
	typedef std::unique_ptr<Song> Ptr;
	Song(const Album* album);
	Song(const std::string& name, const Album* album);

	void download(const std::filesystem::path& path);
	bool isFile();

    std::string name;
    std::string URL;
	int trackNumber;
};

std::ostream& operator << (std::ostream& stream, const Song& song);
