const std = @import("std");
const ArrayList = std.ArrayList;
const String = ArrayList(u8);

pub const metadata = struct {
    album: String.?,
    artist: String.?,
    year: String.?,

    pub fn deinit(self: @This()) void {
        if (self.artist) |art|
            art.deinit();
        if (self.album) |al|
            al.deinit();
        if (self.year) |y|
            y.deinit();
    }
};
