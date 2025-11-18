#include <iostream>
#include <library.hpp>
#include <memory>

namespace fs = std::filesystem;

Iterator Library::begin() {
	return Iterator(mAlbums, mAlbums.begin()->second->songs.begin());
}

Iterator Library::end() {
	auto end = mAlbums.end();
	end--;
	auto songEnd = end->second->songs.end();
	songEnd--;
	return Iterator(mAlbums, end->second->songs.end());
	// return Iterator(mAlbums, songEnd);
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

void Library::addSong(const Song &song) {
	if (mAlbums[song.getAlbum()->name].get() == nullptr) {
		mAlbums[song.getAlbum()->name] =
		    std::make_shared<Album>(*song.getAlbum());
		Song::Ptr s = std::make_shared<Song>(song);
		mAlbums[song.getAlbum()->name]->songs.emplace_back(s);
		return;
	}
	mAlbums[song.getAlbum()->name]->songs.emplace_back(
	    std::make_shared<Song>(song));
}

size_t Library::size() const {
	size_t sum = 0;

	for (const auto &album : mAlbums) {
		sum += mAlbums.size();
	}
	return sum;
}

bool Library::empty() const { return mAlbums.empty(); }

void Library::download(const fs::path &path) {
	for (const auto &[albumName, album] : mAlbums) {
		album->download(path);
	}
}

void Library::addAlbum(const Album::Ptr &album) {
	mAlbums[album->name] = std::make_shared<Album>(*album);
}

const Library::AlbumsType &Library::getAlbums() const { return mAlbums; }
