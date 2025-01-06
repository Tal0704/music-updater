#include <string>
#include <vector>
#include <memory>
#include <filesystem>

struct Song;

struct Album {
	typedef std::unique_ptr<Album> Ptr;
	
	Album();
	Album(const std::string& name);

	void download(const std::filesystem::path& path);
	void populateMetadata(const char* bearer);

	std::vector<std::unique_ptr<Song>> songs;
	std::string name;
	std::string year;
	std::string imageURL;
	std::string artist;
	uint totalSize;
};

std::ostream& operator << (std::ostream& stream, const Album& album);
