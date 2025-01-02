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
std::vector<Song::Ptr> getDownloaded(const std::filesystem::path& path);

void cleanLibrary(std::vector<Song::Ptr>& downloaded, std::vector<Album::Ptr>& library);

void populateAlbumMetadata(Album::Ptr& album);
