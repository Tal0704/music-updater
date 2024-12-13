const std = @import("std");
const ArrayList = std.ArrayList;
const String = ArrayList(u8);

pub const metadata = struct {
    songName: String,
    album: String.?,
    artist: String.?,
    year: String.?,

    pub fn deinit(self: @This()) void {
        self.songName.deinit();
        if (self.artist) |art|
            art.deinit();
        if (self.album) |al|
            al.deinit();
        if (self.year) |y|
            y.deinit();
    }
};
