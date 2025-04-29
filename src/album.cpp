#include <album.hpp>
#include <fstream>
#include <song.hpp>
#include <json.hpp>
#include <iostream>
#include <exec.hpp>
#include <string>
#include <https.hpp>
#include <format>

namespace fs = std::filesystem;
using json = nlohmann::json;

Album::Album() {

}

Album::Album(const std::string& name)
	: name(name)
{

}

std::string cURLCommand(const std::string& URL, const std::string& path) {
	std::string curlImage("curl -s --url \"");
	curlImage += URL;
	curlImage += "\" --output \"";
	curlImage += path;
	curlImage += "/temp.jpg\"";
	return curlImage;
}

void Album::download(const std::filesystem::path& path) {
	exec(cURLCommand(imageURL, path.string()));
	for(auto& song: songs) {
		std::cout << "Downloading: " << this->name << " - " << song->name << "...\n";
		song->download(path);
	}
	std::cout << "\n";
	fs::remove(path.string() + "/temp.jpg");
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
			uri.insert(i + 1, 1, '0');
			uri.insert(i + 1, 1, '2');
		}
	}
	return uri;
}

float calcPercent(float value, uint total) {
	return value / total;
}

float precentAccurate(const std::string& left, const std::string& right) {
	uint totalAccurate = 0;
	uint i = 0;
	for(i = 0; i < std::max(left.size(), right.size()); i++) {
		if(i >= left.size() || i >= right.size()) {
			return calcPercent(totalAccurate, i);
		}
	}
	return calcPercent(totalAccurate, i);
}

char androidify(char c) {
	switch (c) {
		case '*':
		return '+';
		case '?':
		return '_';
		default:
		return c;
	}
}

void Album::populateMetadata() {
	std::string curlCommand = "curl -s --request GET --url \"https://musicbrainz.org/ws/2/release/?query=artist:";
	curlCommand += convertToUri(artist.c_str());
	curlCommand += "%20AND%20release:";
	curlCommand += convertToUri(name.c_str());
	curlCommand += "&fmt=json\"";
	// json data = json::parse(exec(curlCommand));
	auto rootUrl = "https://musicbrainz.org/ws/2/";
	// std::cout << std::format("{}release/?query=artist:{}%20AND%20release:{}&fmt=json", rootUrl, convertToUri(artist.c_str()), convertToUri(name.c_str())) << "\n";
	json data = json::parse(httpsGet(std::format("{}release/?query=artist:{}%20AND%20release:{}&fmt=json", rootUrl, convertToUri(artist.c_str()), convertToUri(name.c_str()))).value());
	// std::cout << data1 << "\n";

	json* correctAlbum = &data["releases"][0];
	for(int i = 0; !correctAlbum->contains("date"); i++) {
		correctAlbum = &data["releases"][i++];
	}

	for(auto& release: data["releases"]) {
		if(release["score"] <= 60)
			continue;
		if(!release.contains("date"))
			continue;
		std::string correctRawDate = correctAlbum->at("date").template get<std::string>();
		if(correctRawDate == "") 
			continue;
		// auto id = release["id"];
		int correctYear = std::stoi(correctRawDate.substr(0, 4));
		try {
			std::string currentRawDate = release.at("date").template get<std::string>();

			if(currentRawDate == "") 
				continue;

			int currentYear = std::stoi(currentRawDate.substr(0, 4));

			if(currentYear < correctYear) {
				correctAlbum = &release;
			}
		}
		catch (...) {
			continue;
		}
	}

	auto& album = *correctAlbum;
	std::string albumID = album["id"].template get<std::string>();
	year = album["date"].template get<std::string>().substr(0, 4);
	year = year.substr(0, 4);
}
