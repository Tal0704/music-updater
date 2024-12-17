const std = @import("std");
const ArrayList = std.ArrayList;
const String = ArrayList(u8);

pub const Metadata = struct {
    album: ?String,
    artist: ?String,
    year: ?String,

    pub fn init() Metadata {
        return @This(){
            .album = null,
            .artist = null,
            .year = null,
        };
    }

    pub fn deinit(self: @This()) void {
        if (self.album) |album|
            album.deinit();
        if (self.artist) |artist|
            artist.deinit();
        if (self.year) |year|
            year.deinit();
    }
};
