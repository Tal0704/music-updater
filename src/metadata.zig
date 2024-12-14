const std = @import("std");
const ArrayList = std.ArrayList;
const String = ArrayList(u8);

pub const Metadata = struct {
    album: ?[]const u8,
    artist: ?[]const u8,
    year: ?[]const u8,
};
