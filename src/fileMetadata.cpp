#include <fileMetadata.hpp>
#include <cassert>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

FileMetadata::FileMetadata(const char* fileName)
	: mFilename(fileName)
{
	assert(avformat_open_input(&mContext, mFilename.c_str(), NULL, NULL) < 0);
	mMetadata = mContext->metadata;
	assert(mMetadata != NULL);
}

FileMetadata::FileMetadata(const std::string& fileName)
	: FileMetadata(fileName.c_str())
{ }

FileMetadata::~FileMetadata() {
	avformat_close_input(&mContext);
}

std::string FileMetadata::read(const std::string& key) const {
	return av_dict_get(mMetadata, key.c_str(), NULL, 0)->value;
}

void FileMetadata::write(const char* key, const char* value) { 
	av_dict_set(&mMetadata, key, value, 0);
}
void FileMetadata::write(const std::string& key, const std::string& value) {
	write(key.c_str(), value.c_str());
}

void FileMetadata::writeInt(const char* key, int64_t value) { 
	av_dict_set_int(&mMetadata, key, value, 0);
}
void FileMetadata::writeInt(const std::string& key, int64_t value) {
	writeInt(key.c_str(), value);
}
