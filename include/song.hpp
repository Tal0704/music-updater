#pragma once
#include <string>
#include <memory>
#include <ostream>

struct Song 
{
	typedef std::unique_ptr<Song> Ptr;
	Song();
	Song(const std::string& name);
    std::string name;

	bool isFile();
};

std::ostream& operator << (std::ostream& stream, const Song& song);
