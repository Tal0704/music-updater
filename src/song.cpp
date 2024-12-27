#include <song.hpp>

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
