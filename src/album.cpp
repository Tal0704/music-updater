#include <album.hpp>
#include <song.hpp>
#include <fstream>
#include <json.hpp>
#include <iostream>
#include <exec.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

Album::Album() {

}

Album::Album(const std::string& name)
	: name(name)
{

}

std::string cURLCommand(const std::string& URL, const std::string& path) {
	std::string curlImage("curl ");
	curlImage += URL;
	curlImage += " --output '";
	curlImage += path;
	curlImage += "/temp.png' -s";
	return curlImage;
}

void Album::download(const std::filesystem::path& path) {
	for(auto& song: songs) {
		std::cout << "Downloading: " << this->name << " - " << song->name << "...\n";
		exec(cURLCommand(imageURL, path.string()));
		song->download(path);
		fs::remove(path.string() + "/temp.png");
	}
}

std::string getSearchTerm(const Album& album)
{
	std::string searchTerm = album.name;
	searchTerm += album.artist;
	std::replace(searchTerm.begin(), searchTerm.end(), ' ', '+');
	return searchTerm;
}

std::string convertToUri(const char* str) {
	std::string uri = str;
	for(uint i = 0; i < uri.length(); i++) {
		if(uri[i] == ' ') {
			uri[i] = '%';
			uri.insert(i, 1, '2');
			uri.insert(i, 1, '0');
		}
	}
	return uri;
}

void Album::populateMetadata() {
	std::string curlCommand = "curl -s --request GET --url \"https://musicbrainz.org/ws/2/release/?query=artist:";
	curlCommand += convertToUri(artist.c_str());
	curlCommand += "%20AND%20release:";
	curlCommand += convertToUri(name.c_str());
	curlCommand += "&fmt=json\" > data.json";
	system(curlCommand.c_str());

	json data = json::parse(std::ifstream("data.json"));

	json* correctAlbum = &data["releases"][0];
	for(auto& release: data["releases"]) {
		std::string correctRawDate = correctAlbum->at("date").template get<std::string>();
		if(correctRawDate == "") 
			continue;
		int correctYear = std::stoi(correctRawDate.substr(0, 4));

		std::string currentRawDate = release.at("date").template get<std::string>();
		if(currentRawDate == "") 
			continue;
		int currentYear = std::stoi(currentRawDate.substr(0, 4));

		if(currentYear < correctYear) {
			correctAlbum = &release;
		}
	}

	auto& album = *correctAlbum;
	std::string albumID = album["id"].template get<std::string>();
	year = album["date"].template get<std::string>().substr(0, 4);
	year = year.substr(0, 4);
	artist = album["artist-credit"][0]["name"].template get<std::string>();
	std::string getImageData = "curl -L --request GET --url https://coverartarchive.org/release/264b958d-e9fd-4cde-9759-e9d40df12a94/front --output a.jpg";
	system(getImageData.c_str());
	std::cout << "here" << "\n";
}
