#pragma once
#include <string>
#include <memory>
#include <ostream>
#include <filesystem>

struct Song 
{
	typedef std::unique_ptr<Song> Ptr;
	Song();
	Song(const std::string& name);

	void download(const std::filesystem::path& path);
	bool isFile();

    std::string name;
    std::string URL;
};

std::ostream& operator << (std::ostream& stream, const Song& song);
