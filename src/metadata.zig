const std = @import("std");
const ArrayList = std.ArrayList;
const String = ArrayList(u8);

pub const metadata = struct {
    album: String,
    artist: String,
    year: String,

    pub fn deinit(self: @This()) void {
        self.artist.deinit();
        self.album.deinit();
        self.year.deinit();
    }
};
