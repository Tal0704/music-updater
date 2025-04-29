#pragma once
#include <string>
#include <optional>
#include <vector>
#include <song.hpp>
#include <fstream>
#include <filesystem>
#include <album.hpp>

std::optional<std::string> getName(const std::string& line);
std::optional<std::string> getLink(const std::string& line);
std::optional<std::string> getAlbum(const std::string& line);
std::optional<std::string> getLink(const std::string& line);

std::vector<Album::Ptr> getLibrary(std::ifstream& inFile);
std::vector<Album::Ptr> getDownloaded(const std::filesystem::path& path);

void organizeSongs(std::vector<Album::Ptr>& library, std::vector<Album::Ptr>& downloaded);
void cleanLibrary(std::vector<Album::Ptr>& library, const std::filesystem::path& path);
