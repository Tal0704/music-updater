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

void Album::populateMetadata(const char* bearer) {
	std::string curlCommand = "curl --request GET --url 'https://api.spotify.com/v1/search?q=";
	
	curlCommand += getSearchTerm(*this);
	curlCommand += "&type=album' --header 'Authorization: Bearer ";
	curlCommand += bearer;
	curlCommand += "' --output data.json -s";
	system(curlCommand.c_str());
		 
	json data = json::parse(std::ifstream("data.json"));
	/* std::cout << data << "\n"; */

	for(auto& albumJson: data["albums"]["items"]) {
		auto albumType = albumJson["album_type"].template get<std::string>();
		auto albumName = albumJson["name"].template get<std::string>();
		std::transform(albumName.begin(), albumName.end(), albumName.begin(), ::tolower);

		std::string albumNameLowerCase;
		std::transform(name.begin(), name.end(), albumNameLowerCase.begin(), ::tolower);

		if(albumType == "album" && albumJson["total_tracks"] == songs.size() && albumName.find(albumNameLowerCase) != std::string::npos) {
			imageURL = albumJson["images"][0]["url"].template get<std::string>();
			year = albumJson["release_date"].template get<std::string>();
			year = year.substr(0, 4);
			artist = albumJson["artists"][0]["name"].template get<std::string>();
			name = albumJson["name"].template get<std::string>();
			std::cout << imageURL << " | " << year << " | " << artist << " | " << name << "\n";
			return;
		}
	}
}
