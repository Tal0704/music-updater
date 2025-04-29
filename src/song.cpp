#include <cctype>
#include <song.hpp>
#include <iostream>
#include <album.hpp>
#include <exec.hpp>
#include <string>
#include <helpers.hpp>

namespace fs = std::filesystem;

Song::Song(Album::Ptr album, Status status)
	: album(album), status(status)
{ }

Song::Song(const std::string& name, Album::Ptr album, Status status)
	:album(album), status(status), name(name)
{ }

std::string yt_dlpCommand(const std::string& URL, const std::string& path) {
	std::string yt_dlp("yt-dlp -x --audio-format mp3 \"");
	yt_dlp += URL;
	yt_dlp += "\" -P ";
	yt_dlp += path;
	yt_dlp += " -o temp -q --cookies-from-browser firefox";
	return yt_dlp;
}

std::string ffmpegCommand(const std::string& path, const Song& song) {
	std::string ffmpeg("ffmpeg -y -i \"");
	std::string songName = song.name;
	androidify(songName);

	ffmpeg += path + "/" + "temp.mp3\" -i \"";
	ffmpeg += path + "/temp.jpg\" ";
	ffmpeg += "-map 0:a -map 1:v -c copy -disposition:v:0 attached_pic ";
	ffmpeg += "-metadata album=\"" + song.album->name + "\" ";
	ffmpeg += "-metadata date=\"" + song.album->year + "\" ";
	ffmpeg += "-metadata artist=\"" + song.album->artist + "\" ";
	ffmpeg += "-metadata track=\"" + std::to_string(song.trackNumber) + "\" ";
	ffmpeg += "-metadata title=\"" + song.name +"\" ";
	ffmpeg += " -loglevel quiet \"";
	ffmpeg += path + "/" + songName + ".mp3\" ";
	return ffmpeg;
}

void Song::download(const fs::path& path) {
	exec(yt_dlpCommand(URL, path.c_str()));
	exec(ffmpegCommand(path, *this));
	fs::remove(path.string() + "/temp.mp3");
}

std::ostream& operator << (std::ostream& stream, const Song::Status& status) {
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
		default:
			break;
	}
	if(status == Status::InBoth) {
	}
	return stream;
}
