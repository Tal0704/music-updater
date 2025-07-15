#pragma once
#include <album.hpp>
#include <optional>
#include <song.hpp>
#include <unordered_map>

class Library;
struct LibraryIterator {
  public:
	typedef Song::Ptr type;
	typedef type *typePointer;
	typedef type &typeReference;
	typedef std::unordered_map<std::string, std::shared_ptr<Album>> map;
	typedef map::iterator mapIterator;
	typedef const mapIterator constIterator;

	LibraryIterator(
	    map &originalMap,
	    const std::optional<Album::ContainerType::iterator> currentIter);

	typeReference operator*() const;
	typePointer operator->() const;

	LibraryIterator &operator++();
	LibraryIterator &operator++(int);

	bool operator==(const LibraryIterator &other) const;
	bool operator!=(const LibraryIterator &other) const;
	LibraryIterator end();

  private:
	mapIterator mMapIter;
	map &mOriginalMap;
	Album::ContainerType::iterator mIter;
	const Album::ContainerType::iterator mEndIter;
};
