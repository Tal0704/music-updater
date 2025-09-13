#pragma once
#include <album.hpp>
#include <list>
#include <map>
#include <optional>
#include <song.hpp>
#include <unordered_map>

// Sends the user message and returns if the answer is 'y' or "Y"
bool confirmUserInput(const std::string &message = "");

// Collects every song that needs to be deleted
std::list<Song::Ptr>
collectToDelete(std::unordered_map<std::string, Album::Ptr> &library,
                const std::unordered_map<std::string, Album::Ptr> &downloaded);

// Collects every song that needs to be downloaded
std::map<std::string, Album::Ptr>
collectToDownload(const std::unordered_map<std::string, Album::Ptr> &library,
                  std::unordered_map<std::string, Album::Ptr> &downloaded);

std::optional<std::string> getName(const std::string &line);
std::optional<std::string> getThumbnail(const std::string &line);
std::optional<std::string> getLink(const std::string &line);
std::optional<std::string> getAlbum(const std::string &line);
std::optional<std::string> getArtist(const std::string &line);

std::string androidify(const std::string &string);
