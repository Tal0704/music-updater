#pragma once
#include <album.hpp>
#include <functional>
#include <list>
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
