#include <album.hpp>
#include <ffmpegCommand.hpp>
#include <format>
#include <utils.hpp>

FfmpegCommnnd::FfmpegCommnnd(Song &song, const std::string &path)
    : mSong(song) {
	mCommand = std::format("ffmpeg -y -i \"{}/temp.mp3\" -i \"{}/temp.png\"",
	                       path, path);

	mCommand = "ffmpeg -y -i \"";

	mCommand += path + "/" + "temp.mp3\" -i \"";
	mCommand += path + "/temp.png\" ";
	mCommand += "-map 0:a -map 1:v -c copy -disposition:v:0 attached_pic ";
	mCommand += "-metadata album=\"" + mSong.getAlbum()->name + "\" ";
	mCommand += "-metadata date=\"" + mSong.getAlbum()->year + "\" ";
	mCommand += "-metadata artist=\"" + mSong.getAlbum()->artist + "\" ";
	mCommand +=
	    "-metadata track=\"" + std::to_string(mSong.getTrackNumber()) + "\" ";
	mCommand += "-metadata genre=\"" + mSong.getAlbum()->genre + "\" ";
	mCommand += "-metadata title=\"" + mSong.getName() + "\" ";
	mCommand += "-metadata comment=\"" + mSong.getURL() + "\" ";
	mCommand += " -loglevel quiet \"";
	mCommand += path + "/" + mSong.getAlbum()->artist + " - " +
	            androidify(song.getName()) + ".mp3\" ";
}
