// #pragma once
// #include <album.hpp>
#include <vector>
// #include <optional>
// #include <song.hpp>

// class Library;
// struct LibraryIterator {
//   public:
// 	typedef Song::Ptr type;
// 	typedef type *typePointer;
// 	typedef type &typeReference;
// 	typedef std::map<std::string, std::shared_ptr<Album>> map;

// 	LibraryIterator(
// 	    map &originalMap,
// 	    const std::optional<Album::ContainerType::iterator> currentIter);

// 	typeReference operator*() const;
// 	typePointer operator->() const;

// 	LibraryIterator &operator++();
// 	LibraryIterator &operator++(int);

// 	bool operator==(const LibraryIterator &other) const;
// 	bool operator!=(const LibraryIterator &other) const;
// 	LibraryIterator end();

//   private:
// 	map::iterator mMapIter;
// 	map &mOriginalMap;
// 	Album::ContainerType::iterator mIter;
// 	const Album::ContainerType::iterator mEndIter;
// };
