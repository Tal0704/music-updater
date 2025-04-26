#pragma once
#include <curl/curl.h>
#include <string>
#include <optional>

std::optional<std::string> httpsGet(const std::string& url);
