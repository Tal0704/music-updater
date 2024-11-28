const std = @import("std");
const music = @import("music.zig");
const ChildProcess = std.process.Child;
const print = std.debug.print;
const fs = std.fs;
const cwd = fs.cwd();
const ArrayList = std.ArrayList;
const String = ArrayList(u8);

pub fn main() anyerror!void {
    var file: fs.File = try cwd.openFile("/home/tal/Documents/Obsidian Vault/music/music.md", .{ .mode = .read_only });
    defer file.close();
    try music.update(&file, std.heap.page_allocator, "/home/tal/Music");
}
