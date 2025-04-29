#include <cstdlib>
#include <curl/curl.h>
#include <https.hpp>
#include <cassert>
#include <iostream>

size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

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

size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	std::string *mem = (std::string *)userp;
	mem->append((char*) contents, realsize);

	return realsize;
}
