#pragma once
#include <string>
#include <optional>
#include <vector>
#include <fstream>
#include <filesystem>

struct Album;

std::optional<std::string> getName(const std::string& line);
std::optional<std::string> getLink(const std::string& line);
std::optional<std::string> getAlbum(const std::string& line);
std::optional<std::string> getLink(const std::string& line);

std::vector<std::shared_ptr<Album>> getLibrary(std::ifstream& inFile);
std::vector<std::shared_ptr<Album>> getDownloaded(const std::filesystem::path& path);

void organizeSongs(std::vector<std::shared_ptr<Album>>& library, std::vector<std::shared_ptr<Album>>& downloaded);
void cleanLibrary(std::vector<std::shared_ptr<Album>>& library, const std::filesystem::path& path);

std::string androidify(const std::string& string);
