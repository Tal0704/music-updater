#include <fileMetadata.hpp>
#include <format>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

FileMetadata::FileMetadata(const char *fileName) : mFilename(fileName) {
	mContext = nullptr;
	if (avformat_open_input(&mContext, mFilename.c_str(), NULL, NULL) != 0) {
		throw std::runtime_error(
		    std::format("Couldn't open {} for reading metadata", mFilename));
	}
	mMetadata = mContext->metadata;

	if (mMetadata == NULL) {
		throw std::runtime_error(
		    std::format("No metadata was found for {}", mFilename));
	}
}

FileMetadata::FileMetadata(const std::string &fileName)
    : FileMetadata(fileName.c_str()) {}

FileMetadata::~FileMetadata() { avformat_close_input(&mContext); }

std::string FileMetadata::read(const std::string &key) const {
	return av_dict_get(mMetadata, key.c_str(), NULL, 0)->value;
}

void FileMetadata::write(const char *key, const char *value) {
	av_dict_set(&mMetadata, key, value, 0);
}
void FileMetadata::write(const std::string &key, const std::string &value) {
	write(key.c_str(), value.c_str());
}

void FileMetadata::writeInt(const char *key, int64_t value) {
	av_dict_set_int(&mMetadata, key, value, 0);
}
void FileMetadata::writeInt(const std::string &key, int64_t value) {
	writeInt(key.c_str(), value);
}
