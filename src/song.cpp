#include <song.hpp>
#include <iostream>
#include <album.hpp>
#include <exec.hpp>

namespace fs = std::filesystem;

Song::Song(const Album* album)
	: album(album)
{

}

Song::Song(const std::string& name, const Album* album)
	:album(album), name(name)
{
}

std::ostream& operator << (std::ostream& stream, const Song& song) {
	stream << song.name;
	return stream;
}

bool Song::isFile() {
	return std::string(name.end() - 4, name.end() - 1) == ".mp";
}

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
	ffmpeg += path + "/" + "temp.mp3\" -i \"";
	ffmpeg += path + "/temp.jpg\" ";
	ffmpeg += "-map 0:a -map 1:v -c copy -disposition:v:0 attached_pic ";
	ffmpeg += "-metadata album=\"" + song.album->name + "\" ";
	ffmpeg += "-metadata date=\"" + song.album->year + "\" ";
	ffmpeg += "-metadata artist=\"" + song.album->artist + "\" ";
	ffmpeg += "-metadata track=\"" + std::to_string(song.trackNumber) + "\" ";
	ffmpeg += " -loglevel quiet \"";
	ffmpeg += path + "/" + song.name + ".mp3\" ";
	return ffmpeg;
}

void Song::download(const fs::path& path) {
	exec(yt_dlpCommand(URL, path.c_str()));
	exec(ffmpegCommand(path, *this));
	fs::remove(path.string() + "/temp.mp3");
}
