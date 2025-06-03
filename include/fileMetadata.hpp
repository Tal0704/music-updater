#pragma once

#include <fstream>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

// TODO: Test if everything works

class FileMetadata {
public:
	std::string read(const std::string& key) const;

	void write(const char* key, const char* value);
	void write(const std::string& key, const std::string& value);
	void writeInt(const char* key, int64_t value);
	void writeInt(const std::string& key, int64_t value);

	FileMetadata(const char* fileName);
	FileMetadata(const std::string& fileName);
	FileMetadata(const std::fstream& file);
	~FileMetadata();

private:
	AVFormatContext* mContext;
	AVDictionary* mMetadata;
	std::string mFilename;
};
