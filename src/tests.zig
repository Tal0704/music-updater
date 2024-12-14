test "deallocating arraylist of songs" {
    var songs = ArrayList(Song).init(testing.allocator);
    defer songs.deinit();
}

const Song = @import("song.zig").Song;
const std = @import("std");
const ArrayList = std.ArrayList;
const String = ArrayList(u8);
const testing = std.testing;
const mem = std.mem;
