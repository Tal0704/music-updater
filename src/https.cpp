#include <cstdlib>
#include <curl/curl.h>
#include <https.hpp>
#include <iostream>
#include <format>
#include <json.hpp>

using json = nlohmann::json;

size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
std::string toParams(const std::string& artist, const std::string& song);

// Function to perform HTTPS GET request
std::optional<std::string> httpsGet(const std::string& url) {
    CURL* handle = curl_easy_init();
    if (handle) {
        std::string chunk;
        // Reserve an initial amount of memory (e.g., 16KB). Adjust this as needed.
        chunk.reserve(16 * 1024);

        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "Music Downloader <0.1>");

        CURLcode res = curl_easy_perform(handle);
        if (res != CURLE_OK) {
            std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(handle);
            return {};
        }

        curl_easy_cleanup(handle);
        return chunk;
    }

    return {};
}

std::optional<std::string> getLyrics(const std::string& artist, const std::string& song, const std::string& apiKey) {
	CURL *handle = curl_easy_init();

	std::string chunk;
	chunk.reserve(8 * 1024);
	curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void* )&chunk);
	curl_easy_setopt(handle, CURLOPT_URL, std::format("https://api.happi.dev/v1/lyrics?{}", toParams(artist, song)).c_str());

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "accept: application/json");
	headers = curl_slist_append(headers, std::format("x-happi-token: {}", apiKey).c_str());
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

	CURLcode res = curl_easy_perform(handle);
	if(res != CURLE_OK) {
		std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
		curl_easy_cleanup(handle);
		return {};
	}

	curl_easy_cleanup(handle);
	json data = json::parse(chunk);
	return data["result"][0]["lyrics"];
}

size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	std::string *mem = (std::string *)userp;
	mem->append((char*) contents, realsize);

	return realsize;
}

std::string urlify(std::string left) {
	while(left.find(' ') != std::string::npos)
		left.replace(left.find(' '), 1, "%20");
	return left;
}

std::string toParams(const std::string& artist, const std::string& song) {
	return std::format("artist={}&track={}", urlify(artist), urlify(song));
}
