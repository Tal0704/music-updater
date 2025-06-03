#pragma once
#include <curl/curl.h>
#include <string>
#include <optional>

std::optional<std::string> httpsGet(const std::string& url);
std::optional<std::string> getLyrics(const std::string& artist, const std::string& song, const std::string& apiKey);
