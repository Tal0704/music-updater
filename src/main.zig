const std = @import("std");
const music = @import("music.zig");
const print = std.debug.print;
const fs = std.fs;
const allocator = std.heap.page_allocator;

pub fn main() anyerror!void {
    var file: fs.File = undefined;
    if (fs.cwd().openFile("/home/tal/Documents/Obsidian Vault/music/music.md", .{ .mode = .read_only })) |openedFile| {
        file = openedFile;
    } else |err| {
        print("Couldn't open music source file\n{}\n", .{err});
        return;
    }
    defer file.close();

    music.update(&file, std.heap.page_allocator, "/home/tal/Music") catch |err| print("Couldn't update music files\n{}\n", .{err});
}
