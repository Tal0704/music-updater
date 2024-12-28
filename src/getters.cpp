#include <getters.hpp>
#include <algorithm>

std::optional<std::string> getName(const std::string& line) {
	if(line.length() == 0 || line[0] != '[')
		return {};
	int i = 1;
	while(line[i] != ']' && uint(i) < line.length()) 
		i++;
	return std::string(line.begin() + 1, line.begin() + i);
}

std::optional<std::string> getLink(const std::string& line) {
	if(line.length() == 0 || line[0] != '[')
		return {};
	int i = 1;
	while(line[i] != '(' && uint(i) < line.length()) 
		i++;
	int j = i;
	while (line[j] != ')' && uint(i) < line.length())
		j++;

	return std::string(line.begin() + i + 1, line.begin() + j);
}

std::optional<std::string> getAlbum(const std::string& line) {
	if(line.length() == 0 || line.find_first_of("##") != 0)
		return {};

	return std::string(line.begin() + 3, line.end());
}

std::vector<Song::Ptr> getLibrary(std::ifstream& inFile) {
	std::string line;
	std::vector<Song::Ptr> songs;
	while(std::getline(inFile, line)) {
		Song::Ptr song = std::make_unique<Song>();
		if(auto name = getName(line))
			song->name = name.value();

		if(song->name.length() > 0)
			songs.emplace_back(std::move(song));
	}
	return songs;
}

std::vector<Song::Ptr> getDownloaded(const fs::path& path) {
	std::vector<Song::Ptr> songs;
	for(const auto& pathIt: fs::directory_iterator(path)) {
		auto lastSlash = pathIt.path().string().find_last_of('/');
		auto pathstr = pathIt.path().string();
		Song::Ptr song = std::make_unique<Song>(std::string(pathstr.begin() + lastSlash + 1, pathstr.end()));

		songs.emplace_back(std::move(song));
	}

	return songs;
}

void cleanDuplicateSongs(std::vector<Song::Ptr>& songs) {
	for(auto it = songs.begin(); it != songs.end();) {
		auto& song = *it;
		auto found = std::find_if(songs.begin(), songs.end(), [&](Song::Ptr& s) -> bool {
					if(song->isFile())
					{
						return std::string(song->name.begin(), song->name.end() - 4) == s->name;
					}
					return false;
				});

		if(found != songs.end()) {
			songs.erase(found);
		}
		else {
			++it;
		}
	}
}
