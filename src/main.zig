const std = @import("std");
const music = @import("music.zig");
const print = std.debug.print;
const fs = std.fs;
const allocator = std.heap.page_allocator;

pub fn main() anyerror!void {
    var file: fs.File = try fs.cwd().openFile("/home/tal/Documents/Obsidian Vault/music/music.md", .{ .mode = .read_only });
    defer file.close();
    try music.update(&file, std.heap.page_allocator, "/home/tal/Music");
}
