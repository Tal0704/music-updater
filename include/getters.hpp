#pragma once
#include <string>
#include <optional>
#include <vector>
#include <song.hpp>
#include <fstream>
#include <filesystem>

std::optional<std::string> getName(const std::string& line);

std::optional<std::string> getLink(const std::string& line);

std::optional<std::string> getAlbum(const std::string& line);

std::vector<Song::Ptr> getLibrary(std::ifstream& inFile);

std::vector<Song::Ptr> getDownloaded(const std::filesystem::path& path);

void cleanLibrary(std::vector<Song::Ptr>& songs);

void deleteUnneededSongs(std::vector<Song::Ptr>& downloaded, std::vector<Song::Ptr>& library, const std::filesystem::path& path);

void populateSong(Song::Ptr& song);

