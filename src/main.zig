const std = @import("std");
const music = @import("music.zig");
const Album = @import("album.zig").Album;
const print = std.debug.print;
const fs = std.fs;
const allocator = std.heap.page_allocator;
const cwd = fs.cwd();

pub fn main() anyerror!void {
    var file: fs.File = undefined;
    if (fs.cwd().openFile("/home/tal/Documents/Obsidian Vault/music/music.md", .{ .mode = .read_only })) |openedFile| {
        file = openedFile;
    } else |err| {
        print("Couldn't open music source file\n{}\n", .{err});
        return;
    }
    defer file.close();

    const musicPath = "/home/tal/Music";
    var musicFolder: fs.Dir = try cwd.openDir(musicPath, .{ .iterate = true });
    defer musicFolder.close();

    music.update(&file, std.heap.page_allocator, &musicFolder, musicPath) catch |err| print("Couldn't update music files\n{}\n", .{err});
}
