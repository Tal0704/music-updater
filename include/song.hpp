#pragma once
#include <string>
#include <memory>
#include <ostream>
#include <filesystem>
#include <metadata.hpp>

struct Song 
{
	typedef std::unique_ptr<Song> Ptr;
	Song();
	Song(const std::string& name);
	Song(const std::string& name, const std::string& URL);

	void download(const std::filesystem::path& path);
	bool isFile();

    std::string name;
    std::string URL;
	Metadata metadata;
};

std::ostream& operator << (std::ostream& stream, const Song& song);
