#include <album.hpp>
#include <exec.hpp>
#include <format>
#include <iostream>
#include <song.hpp>
#include <string>
#include <utils.hpp>

namespace fs = std::filesystem;

Song::Song(const std::string &name, Album::Ptr album, Status status)
    : mAlbum(album), mStatus(status), mName(name) {}

std::string yt_dlpCommand(const std::string &URL, const std::string &path) {
	std::string yt_dlp("yt-dlp -x --audio-format mp3 \"");
	yt_dlp += URL;
	yt_dlp += "\" -P ";
	yt_dlp += path;
	yt_dlp += " -o temp -q --cookies-from-browser firefox";
	return yt_dlp;
}

std::string ffmpegCommand(const std::string &path, const Song &song) {
	std::string ffmpeg("ffmpeg -y -i \"");
	std::string songName = song.getName();

	ffmpeg += path + "/" + "temp.mp3\" -i \"";
	ffmpeg += path + "/temp.png\" ";
	ffmpeg += "-map 0:a -map 1:v -c copy -disposition:v:0 attached_pic ";
	ffmpeg += "-metadata album=\"" + song.getAlbum()->name + "\" ";
	ffmpeg += "-metadata date=\"" + song.getAlbum()->year + "\" ";
	ffmpeg += "-metadata artist=\"" + song.getAlbum()->artist + "\" ";
	ffmpeg +=
	    "-metadata track=\"" + std::to_string(song.getTrackNumber()) + "\" ";
	ffmpeg += "-metadata genre=\"" + song.getAlbum()->genre + "\" ";
	ffmpeg += "-metadata title=\"" + song.getName() + "\" ";
	ffmpeg += "-metadata url=\"" + song.getURL() + "\" ";
	ffmpeg += " -loglevel quiet \"";
	ffmpeg +=
	    path + "/" + song.getAlbum()->artist + " - " + songName + ".mp3\" ";
	return ffmpeg;
}

void Song::download(const fs::path &path) {
	exec(std::format("yt-dlp -x --audio-format mp3 \"{}\" -P {} -o temp -q "
	                 "--cookies-from-browser firefox -N 20",
	                 mURL, path.c_str()));
	exec(ffmpegCommand(path, *this));
	fs::remove(path.string() + "/temp.mp3");
}

std::ostream &operator<<(std::ostream &stream, const Song &song) {
	stream << *(song.getAlbum()) << " - " << song.getName();
	return stream;
}

std::ostream &operator<<(std::ostream &stream, const Song::Status &status) {
	using Status = Song::Status;
	switch (status) {
	case Status::InBoth:
		stream << "In Both";
		break;
	case Status::Downloaded:
		stream << "Downloaded";
		break;
	case Status::Library:
		stream << "Library";
		break;
	case Status::DifferentUrl:
		stream << "DifferentUrl";
		break;
	default:
		break;
	}
	if (status == Status::InBoth) {
	}
	return stream;
}

bool Song::operator==(const Song &o) {
	return (mName == o.mName) && (mAlbum->name == o.mAlbum->name) &&
	       (mAlbum->artist == o.mAlbum->artist);
}

std::shared_ptr<Album> Song::getAlbum() const { return mAlbum; }
std::string Song::getURL() const { return mURL; }
std::string Song::getAlternateUrl() const { return mAlternateUrl; }
std::string Song::getName() const { return mName; }
int Song::getTrackNumber() const { return mTrackNumber; };
Song::Status Song::getStatus() const { return mStatus; }

void Song::setAlbum(const std::shared_ptr<Album> &album) { mAlbum = album; }
void Song::setURL(const std::string &URL) { mURL = URL; }
void Song::setAlternateUrl(const std::string &URL) { mAlternateUrl = URL; }
void Song::setName(const std::string &name) { mName = name; }
void Song::setTrackNumber(const int &trackNumber) {
	mTrackNumber = trackNumber;
}
void Song::setStatus(const Status &status) { mStatus = status; }

std::string Song::toString() const {
	return std::format("{} - {}", mAlbum->artist, mName);
}

std::string Song::toFile() const {
	std::string name = androidify(mName);
	std::string album = androidify(mAlbum->name);
	return std::format("{} - {}.mp3", mAlbum->artist, name);
}
