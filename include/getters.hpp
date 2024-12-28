#pragma once
#include <string>
#include <optional>
#include <vector>
#include <song.hpp>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::optional<std::string> getName(const std::string& line);

std::optional<std::string> getLink(const std::string& line);

std::optional<std::string> getAlbum(const std::string& line);

std::vector<Song::Ptr> getLibrary(std::ifstream& inFile);

std::vector<Song::Ptr> getDownloaded(const fs::path& path);

void cleanDuplicateSongs(std::vector<Song::Ptr>& songs);
