#pragma once
#include <string>
#include <memory>
#include <ostream>
#include <filesystem>

struct Album;

struct Song 
{
public:
	typedef std::unique_ptr<Song> Ptr;

	enum Status {
		InBoth = 0 << 0,
		Downloaded = 1 << 0,
		Library = 1 << 1,
	};

	Song(const std::string& name, std::shared_ptr<Album> album, Status status = Status::InBoth);

	void download(const std::filesystem::path& path);

	std::shared_ptr<Album> album;
    std::string URL;
	int trackNumber;
	Status status;
    std::string name;
};

bool operator==(const Song& left, const Song& right);

std::ostream& operator << (std::ostream& stream, const Song::Status& status);
