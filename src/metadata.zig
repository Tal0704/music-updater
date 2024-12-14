const std = @import("std");
const ArrayList = std.ArrayList;
const String = ArrayList(u8);

pub const Metadata = struct {
    album: ?[]const u8,
    artist: ?[]const u8,
    year: ?[]const u8,
    allocator: std.mem.Allocator,

    pub fn deinit(self: @This()) void {
        if (self.album) |album|
            self.allocator.free(album);
        if (self.artist) |artist|
            self.allocator.free(artist);
        if (self.year) |year|
            self.allocator.free(year);
    }
};
