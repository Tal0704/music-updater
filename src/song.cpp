#include <song.hpp>
#include <iostream>
#include <array>

namespace fs = std::filesystem;

Song::Song()
	: name("")
{

}

Song::Song(const std::string& name)
	:name(name)
{

}

std::ostream& operator << (std::ostream& stream, const Song& song) {
	stream << song.name;
	return stream;
}

bool Song::isFile() {
	return std::string(name.end() - 4, name.end() - 1) == ".mp";
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    // Open the command for reading
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    // Read the output a line at a time - output it
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void Song::download(const fs::path& path) {
	std::string s("yt-dlp -x --audio-format mp3 ");
	s += URL;
	s += " -P ";
	s += path.c_str();
	s += " -o \"";
	s += name;
	s += "\" -q";

	auto c = exec(s.c_str());
	std::cout << "Command: " << s << "\n";

}


