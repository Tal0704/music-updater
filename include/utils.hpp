#pragma once
#include <album.hpp>
#include <library.hpp>
#include <list>
#include <optional>
#include <song.hpp>

// Sends the user message and returns if the answer is 'y' or "Y"
bool confirmUserInput(const std::string &message = "");

// Collects every song that needs to be deleted
std::list<Song::Ptr> collectToDelete(Library &library, Library &downloaded);

std::vector<Song::Ptr> collectUrls(Library &library, Library &downloaded);
// Collects every song that needs to be downloaded
Library collectToDownload(Library &library, Library &downloaded);

std::optional<std::string> getName(const std::string &line);
std::optional<std::string> getThumbnail(const std::string &line);
std::optional<std::string> getLink(const std::string &line);
std::optional<std::string> getAlbum(const std::string &line);
std::optional<std::string> getArtist(const std::string &line);

std::string androidify(const std::string &string);
