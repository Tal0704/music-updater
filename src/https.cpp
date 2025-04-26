#include <cstdlib>
#include <curl/curl.h>
#include <https.hpp>
#include <cassert>
#include <iostream>

size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

std::optional<std::string> httpsGet(const std::string& url) {
	CURL* handle = curl_easy_init();
	if(handle) {
		std::string chunk;
		curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
		curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
		curl_easy_setopt(handle, CURLOPT_USERAGENT, "Music Downloaded <0.1>");

		CURLcode res = curl_easy_perform(handle);
		if(res != CURLE_OK) {
			std::cout << "Error: " << curl_easy_strerror(res);
			return {};
			std::abort();
		} else {
			return chunk;
		}
		curl_easy_cleanup(handle);
	}
	
	return {};
}

size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	std::string *mem = (std::string *)userp;
	(*mem) += (char*) contents;

	// char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
	// if(ptr == NULL) {
	// printf("error: not enough memory\n");
	// return 0;
	// }

	// mem->memory = ptr;
	// memcpy(&(mem->memory[mem->size]), contents, realsize);
	// mem->size += realsize;
	// mem->memory[mem->size] = 0;

	return realsize;
}
