#pragma once
#include <fileMetadata.hpp>
#include <filesystem>
#include <memory>
#include <ostream>
#include <string>

struct Album;

class Song {
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

	std::shared_ptr<Album> getAlbum() const;
	std::string getURL() const;
	std::string getAlternateUrl() const;
	std::string getName() const;
	int getTrackNumber() const;
	Status getStatus() const;

  private:
	std::shared_ptr<Album> album;
	std::string URL;
	std::string alternateUrl;
	int trackNumber;
	Status status;
	std::string name;
};

bool operator==(const Song &left, const Song &right);

std::ostream &operator<<(std::ostream &stream, const Song::Status &status);
