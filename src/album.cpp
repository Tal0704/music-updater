#include <album.hpp>
#include <fstream>
#include <song.hpp>
#include <json.hpp>
#include <iostream>
#include <exec.hpp>
#include <stdexcept>
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

void Album::download(const std::filesystem::path& path) {
	std::cout << "Downloading " << name << std::endl;
	auto res = httpsGet(imageURL);
	if(!res.has_value()) {
		throw std::runtime_error(std::format("No respone from: {}", imageURL));
	}

	std::ofstream image(path.string() + "/temp.png");
	image << res.value();

	for(auto& song: songs) {
		std::cout << "Downloading: " << this->name << " - " << song->name << "...\n";
		song->download(path);
	}
	std::cout << "\n";
	fs::remove(path.string() + "/temp.png");
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

void populateSongsMetadata(const std::vector<std::unique_ptr<Song>>& songs, const std::string& apiKey) {
	for(auto& song: songs) {
		auto lyrics = getLyrics(song->album->artist, song->name, apiKey).value_or(""); 
		song->lyrics = getLyrics(song->album->artist, song->name, apiKey).value_or("");
		std::cout << song->name << " lyrics: " << song->lyrics << std::endl;
	}
}

void Album::populateMetadata(const std::string& apiKey) {
	populateSongsMetadata(songs, apiKey);
	constexpr auto rootUrl = "https://musicbrainz.org/ws/2/";
#ifndef NDEBUG
	std::cout << std::format("{}release/?query=artist:{}%20AND%20release:{}&fmt=json", rootUrl, convertToUri(artist.c_str()), convertToUri(name.c_str())) << std::endl;
#endif

	json data = json::parse(httpsGet(std::format("{}release/?query=artist:{}%20AND%20release:{}&fmt=json", rootUrl, convertToUri(artist.c_str()), convertToUri(name.c_str()))).value());

	json* correctAlbum = &data["releases"][0];
	for(uint i = 0; !correctAlbum->contains("date"); i++) {
		if (i >= data["releases"].size())
			throw std::runtime_error(std::format("Error, can't find date for {} - {}", artist, name));
		correctAlbum = &data["releases"][i++];
	}

	for(auto& release: data["releases"]) {
		if(release["score"] <= 70)
			continue;
		if(!release.contains("date"))
			continue;
		std::string correctRawDate = correctAlbum->at("date").template get<std::string>();
		if(correctRawDate == "") 
			continue;

		if (genre == ""){
			auto id = release["id"].template get<std::string>();
			auto ge = json::parse(httpsGet(std::format("{}release/{}?inc=genres&fmt=json", rootUrl, id)).value());
			if (!ge["genres"].empty())
				genre = ge["genres"][0]["name"].template get<std::string>();
		}

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
	year = album["date"].template get<std::string>().substr(0, 4);
	year = year.substr(0, 4);
}
