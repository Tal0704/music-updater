#pragma once
#include <album.hpp>
#include <functional>
#include <list>
#include <optional>
#include <song.hpp>
#include <unordered_map>

void confirmUserInput(std::function<void(void)> callback);

// Collects every song that needs to be deleted
std::list<Song::Ptr>
collectToDelete(const std::unordered_map<std::string, Album::Ptr> &library,
                const std::unordered_map<std::string, Album::Ptr> &downloaded);

// Collects every song that needs to be downloaded
std::list<Song::Ptr> collectToDownload(
    const std::unordered_map<std::string, Album::Ptr> &library,
    const std::unordered_map<std::string, Album::Ptr> &downloaded);

std::optional<std::string> getName(const std::string &line);
std::optional<std::string> getThumbnail(const std::string &line);
std::optional<std::string> getLink(const std::string &line);
std::optional<std::string> getAlbum(const std::string &line);
std::optional<std::string> getArtist(const std::string &line);
