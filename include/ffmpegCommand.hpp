#include <song.hpp>
#include <string>

class FfmpegCommnnd {
  public:
	FfmpegCommnnd(Song &song, const std::string &path);
	std::string &operator()() { return mCommand; };
	FfmpegCommnnd &operator()(const std::string &field,
	                          const std::string &value);

  private:
	std::string mCommand;
	const Song &mSong;
};
