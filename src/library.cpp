#include <iostream>
#include <library.hpp>
#include <memory>

Library::iterator Library::begin() {
	std::cout << &*mAlbums.end() << std::endl;
	return LibraryIterator(mAlbums, {});
}

const Library::iterator Library::end() {
	AlbumsType::iterator albumIt = mAlbums.begin();
	std::cout << &*mAlbums.begin() << std::endl;
	Album::ContainerType::iterator it = albumIt->second->songs.begin();
	while (albumIt != mAlbums.end()) {
		++it;
		if (it == albumIt->second->songs.end()) {
			++albumIt;
		}
	}
	return LibraryIterator(mAlbums, it);
}

std::ostream &operator<<(std::ostream &stream, const Library &lib) {
	for (auto &[albumName, album] : lib.mAlbums) {
		stream << albumName << ":\n";
		for (auto &song : album->songs) {
			stream << *song << "\n";
		}
	}

	return stream;
}

std::shared_ptr<Album> &Library::operator[](const std::string &key) {
	return mAlbums[key];
}

void Library::removeSong(const Song *song) {
	auto &songs = mAlbums[song->getAlbum()->name]->songs;
	std::erase_if(songs,
	              [&song](auto &other) -> bool { return song == other.get(); });
}
