#pragma once
#include <album.hpp>
// #include <libraryIterator.hpp>
#include <filesystem>
#include <map>
#include <optional>
#include <song.hpp>

class Library;
struct Iterator {
  public:
	typedef Song::Ptr type;
	typedef type *typePointer;
	typedef type &typeReference;
	typedef std::map<std::string, std::shared_ptr<Album>> AlbumsType;

	Iterator(AlbumsType &originalMap,
	         const std::optional<Album::ContainerType::iterator> currentIter);

	typeReference operator*() const;
	typePointer operator->() const;

	Iterator &operator++();
	Iterator &operator++(int);

	bool operator==(const Iterator &other) const;
	bool operator!=(const Iterator &other) const;
	Iterator end();

  private:
	Album::ContainerType::iterator mCurrent;
	AlbumsType::iterator mCurrentAlbum;
	AlbumsType &mOriginalMap;
};

class Library {
  public:
	typedef std::map<std::string, std::shared_ptr<Album>> AlbumsType;
	Iterator begin();
	Iterator end();

	std::shared_ptr<Album> &operator[](const std::string &key);
	const AlbumsType &getAlbums() const;
	void removeSong(const Song *song);
	void addSong(const Song &song);
	void addAlbum(const Album::Ptr &album);
	void download(const std::filesystem::path &path);
	size_t size() const;
	bool empty() const;

  private:
	AlbumsType mAlbums;

	friend std::ostream &operator<<(std::ostream &stream, const Library &lib);
};
std::ostream &operator<<(std::ostream &stream, const Library &lib);
