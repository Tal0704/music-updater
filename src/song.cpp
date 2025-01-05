#include <song.hpp>
#include <iostream>
#include <array>
#include <album.hpp>

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

std::string exec(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    // Open the command for reading
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    // Read the output a line at a time - output it
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::string yt_dlpCommand(const std::string& URL, const std::string& path) {
	std::string yt_dlp("yt-dlp -x --audio-format mp3 ");
	yt_dlp += URL;
	yt_dlp += " -P ";
	yt_dlp += path;
	yt_dlp += " -o temp -q";
	return yt_dlp;
}

std::string cURLCommand(const std::string& URL, const std::string& path, const std::string& name) {
	std::string curlImage("curl ");
	curlImage += URL;
	curlImage += " --output ";
	curlImage += path;
	curlImage += "/";
	curlImage += name;
	curlImage += ".png";
	return curlImage;
}

std::string ffmpegCommand(const std::string& path, const Song& song) {
	std::string ffmpeg("ffmpeg -y -i ");
	ffmpeg += path + "/" + "temp.mp3 -i ";
	ffmpeg += path + "/" + song.name + ".png ";
	ffmpeg += "-map 0:a -map 1:v -c copy -disposition:v:0 attached_pic ";
	ffmpeg += "-metadata album='" + song.album->name + "' ";
	/* ffmpeg += "-metadata date='" + song.album->year + "' "; */
	ffmpeg += "-metadata artist='" + song.album->artist + "' ";
	ffmpeg += "-metadata track='" + std::to_string(song.trackNumber) + "' ";
	ffmpeg += " -loglevel quiet ";
	ffmpeg += path + "/" + song.name + ".mp3 ";
	return ffmpeg;
}

void Song::download(const fs::path& path) {
	exec(yt_dlpCommand(URL, path.c_str()));
	std::cout << album->imageURL << " | " << album->year << " | " << album->artist << " | " << album->name << "\n";
	exec(cURLCommand(album->imageURL, path.string(), name));
	exec(ffmpegCommand(path.c_str(), *this));
	fs::remove(path.string() + "/temp.mp3");
	fs::remove(path.string() + "/" + name + ".png");
}
