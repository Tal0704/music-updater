#pragma once
#include <album.hpp>
#include <libraryIterator.hpp>
#include <song.hpp>
#include <unordered_map>

class Library {
  public:
	typedef LibraryIterator iterator;
	typedef std::unordered_map<std::string, std::shared_ptr<Album>> AlbumsType;
	iterator begin();
	const iterator end();

	std::shared_ptr<Album> &operator[](const std::string &key);
	void removeSong(const Song *song);

  private:
	AlbumsType mAlbums;

	friend std::ostream &operator<<(std::ostream &stream, const Library &lib);
};
std::ostream &operator<<(std::ostream &stream, const Library &lib);
