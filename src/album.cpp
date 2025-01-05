#include <album.hpp>
#include <song.hpp>
#include <fstream>
#include <json.hpp>
#include <iostream>

using json = nlohmann::json;

Album::Album() {

}

Album::Album(const std::string& name)
	: name(name)
{

}

void Album::download(const std::filesystem::path& path) {
	for(auto& song: songs) {
		std::cout << "Downloading: " << this->name << " - " << song->name << "...\n";
		song->download(path);
	}
}

std::string getSearchTerm(const Album& album)
{
	std::string searchTerm = album.name;
	searchTerm += album.artist;
	std::replace(searchTerm.begin(), searchTerm.end(), ' ', '+');
	return searchTerm;
}

void Album::populateMetadata() {
	auto *album = this;
	std::string curlCommand = "curl --request GET --url 'https://api.spotify.com/v1/search?q=";
	
	curlCommand += getSearchTerm(*album);
	curlCommand += "&type=album' "
  "--header 'Authorization: Bearer BQApKxThSlaRlC2lvs36cS523NvmVL-dNLi-9GIIgreFX0QUp3G9pZYjhW1aQR0O5fJOtchmgbgcfbVh4QhoojSLIeL9ZrN0xmz_wSr9u8Bp3jLp7TM' --output data.json -s";
	system(curlCommand.c_str());
		 
	json data = json::parse(std::ifstream("data.json"));

	for(auto& albumJson: data["albums"]["items"]) {
		auto albumType = albumJson["album_type"].template get<std::string>();
		auto albumName = albumJson["name"].template get<std::string>();
		std::transform(albumName.begin(), albumName.end(), albumName.begin(), ::tolower);

		std::string albumNameLowerCase;
		std::transform(album->name.begin(), album->name.end(), albumNameLowerCase.begin(), ::tolower);

		if(albumType == "album" && albumJson["total_tracks"] == album->songs.size() && albumName.find(albumNameLowerCase) != std::string::npos) {
			album->imageURL = albumJson["images"][0]["url"].template get<std::string>();
			album->year = albumJson["release_date"].template get<std::string>();
			album->year = album->year.substr(0, 4);
			album->artist = albumJson["artists"][0]["name"].template get<std::string>();
			album->name = albumJson["name"].template get<std::string>();
			return;
		}
	}
}
