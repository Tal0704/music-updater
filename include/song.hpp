#pragma once
#include <fileMetadata.hpp>
#include <filesystem>
#include <memory>
#include <ostream>
#include <string>

struct Album;

class Song {
  public:
	typedef std::shared_ptr<Song> Ptr;

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

	void setAlbum(const std::shared_ptr<Album> &album);
	void setURL(const std::string &URL);
	void setAlternateUrl(const std::string &url);
	void setName(const std::string &name);
	void setTrackNumber(const int &trackNumber);
	void setStatus(const Status &status);

	bool operator==(const Song &other);

	std::string toString() const;
	std::string toFile() const;

  private:
	std::shared_ptr<Album> mAlbum;
	std::string mURL;
	std::string mAlternateUrl;
	int mTrackNumber;
	Status mStatus;
	std::string mName;
};

std::ostream &operator<<(std::ostream &stream, const Song &song);
bool operator==(const Song &left, const Song &right);

std::ostream &operator<<(std::ostream &stream, const Song::Status &status);
