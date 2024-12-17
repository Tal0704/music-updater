pub fn update(file: *fs.File, allocator: mem.Allocator, dir: *fs.Dir, musicPath: []const u8) !void {
    var songs: ArrayList(*Song) = ArrayList(*Song).init(allocator);
    defer songs.deinit();

    try readLibrary(&songs, file, allocator);
    try readFiles(&songs, dir, musicPath, allocator);

    try deleteSongs(&songs);

    downloadSongs(&songs, musicPath);
}

fn readFiles(songs: *ArrayList(*Song), musicFolder: *fs.Dir, musicPath: []const u8, allocator: mem.Allocator) !void {
    var walker = try musicFolder.walk(allocator);
    defer walker.deinit();
    while (try walker.next()) |file| {
        var song: *Song = try allocator.create(Song);
        song.* = Song.init(allocator);
        song.name = try song.allocator.alloc(u8, file.path.len - 4);

        mem.copyForwards(u8, song.name, file.path[0 .. file.path.len - 4]);

        song.isToDelete = isDeleteSong(songs, song);
        if (song.isToDelete) {
            song.path = try song.allocator.alloc(u8, musicPath.len);
            mem.copyForwards(u8, song.path.?, musicPath);
        }

        if (song.isToDelete) {
            try songs.append(song);
        } else {
            for (songs.items) |s| {
                if (mem.eql(u8, s.name, song.name)) {
                    s.isDownloaded = true;
                }
            }
            song.deinit();
            allocator.destroy(song);
        }
    }
}

fn readLibrary(songs: *ArrayList(*Song), file: *fs.File, allocator: std.mem.Allocator) !void {
    var buffReader = std.io.bufferedReader(file.reader());
    var inStream = buffReader.reader();
    var buf: [1024]u8 = undefined;
    while (try inStream.readUntilDelimiterOrEof(&buf, '\n')) |line| {
        if (line.len == 0 or line[0] == '-') continue;

        var song: *Song = try allocator.create(Song);
        song.* = Song.init(allocator);

        if (getName(line)) |name| {
            song.name = try song.allocator.alloc(u8, name.len);
            mem.copyForwards(u8, song.name, name);
        } else {
            song.deinit();
            allocator.destroy(song);
            continue;
        }

        if (getURL(line)) |URL| {
            song.URL = try song.allocator.alloc(u8, URL.len);
            mem.copyForwards(u8, song.URL.?, URL);
        }

        try songs.append(song);
    }
}

fn isDeleteSong(songs: *ArrayList(*Song), song: *Song) bool {
    for (songs.items) |currentSong| {
        if (mem.eql(u8, song.name, currentSong.name))
            return false;
    }

    return true;
}

fn deleteSongs(songs: *ArrayList(*Song)) !void {
    var count: usize = 0;
    for (songs.items) |song| {
        if (song.isToDelete) count += 1;
    }
    if (count == 0) {
        print("No songs to delete :D\n", .{});
        return;
    }

    print("Are you sure you want to delete:\n", .{});
    for (songs.items) |song| {
        if (song.isToDelete) {
            print("{s}\n", .{song.name});
        }
    }

    print("y/N\n", .{});

    var buff: [10]u8 = undefined;
    const stdin = std.io.getStdIn().reader();
    const input = try stdin.readUntilDelimiter(buff[0..], '\n');

    if (!mem.eql(u8, input, "y")) {
        print("Not deleteing!\n", .{});
        return;
    }

    for (songs.items) |song| {
        if (song.isToDelete) {
            song.delete() catch |err| switch (err) {
                error.PathNotFound => print("Couldn't find path for {s}\n", .{song.name}),
                else => print("Couldn't delete file: {s}, {}\n", .{ song.name, err }),
            };
        }
    }
}

fn downloadSongs(songs: *ArrayList(*Song), dir: []const u8) void {
    var count: usize = 0;
    for (songs.items) |song| {
        if (!song.isDownloaded)
            count += 1;
    }
    if (count == 0) {
        print("No songs to download!\n", .{});
        return;
    }

    print("Downloading songs...\n", .{});
    for (songs.items) |song| {
        if (!song.isToDelete) {
            if (song.download(dir)) {
                print("Downloaded {s} succefully!\n", .{song.name});
            } else |err| {
                print("Couldn't download {s}: {}\n", .{ song.name, err });
            }
        }
    }
}

fn getName(line: []const u8) ?[]const u8 {
    if (line.len == 0 or line[0] != '[')
        return null;
    var i: usize = 0;
    while (line[i] != ']') {
        i += 1;
    }

    return line[1..i];
}

fn getURL(line: []const u8) ?[]const u8 {
    if (line.len == 0 or line[0] != '[')
        return null;
    var i: usize = 0;
    while (line[i] != ']')
        i += 1;

    while (line[i] != '(') {
        i += 1;
    }

    var j: usize = i;
    while (line[j] != ')') {
        j += 1;
    }

    return line[i + 1 .. j];
}

fn getAlbum(line: []const u8) ?[]const u8 {
    if (!mem.eql(u8, line[0..2], "## "))
        return null;
    var i: usize = 4;
    while (line[i] != ']') {
        i += 1;
    }
    return line[4..i];
}

fn getYear(line: []const u8) ?[]const u8 {
    if (!mem.eql(u8, line[0..2], "## "))
        return null;
    var i: usize = 4;
    while (line[i] != ']') {
        i += 1;
    }

    while (line[i] != '(') {
        i += 1;
    }
    var j = i;
    while (line[j] != ')') {
        j += 1;
    }

    return line[i..j];
}

fn getArtist(line: []const u8) ?[]const u8 {
    if (!mem.eql(u8, line[0..1], "# "))
        return null;
    return line[4..];
}

test "Get File Name from Line" {
    const line = "[Metallica - Master of Puppets](https://www.youtube.com/watch?v=mOOzcgDLCbQ&list=WL&index=58)";
    const name = getName(line);

    if (name) |n|
        try expect(mem.eql(u8, n, "Metallica - Master of Puppets"));
}

test "Get URL from Line" {
    const line = "[Metallica - Master of Puppets](https://www.youtube.com/watch?v=mOOzcgDLCbQ&list=WL&index=58)";
    const URL = getURL(line);

    if (URL) |u|
        try expect(mem.eql(u8, u, "https://www.youtube.com/watch?v=mOOzcgDLCbQ&list=WL&index=58"));
}

test "album" {
    const line = "## [Master of Puppets](1968)";
    const album = getAlbum(line);
    if (album) |a|
        try expect(mem.eql(u8, a, "Master of Puppets"));
}

test "year" {
    const line = "## [Master of Puppets](1968)";
    const year = getYear(line);
    if (year) |a|
        try expect(mem.eql(u8, a, "1968"));
}

test "artist" {
    const line = "# Metallica";
    if (getArtist(line)) |a|
        try expect(mem.eql(u8, a, "Metallica"));
}

test "read files" {
    var musicFolder: fs.Dir = try cwd.openDir("/home/tal/Music", .{ .iterate = true });
    defer musicFolder.close();

    var songs: ArrayList(*Song) = ArrayList(*Song).init(std.testing.allocator);
    defer songs.deinit();
    try readFiles(&songs, &musicFolder, "/home/tal/Music", std.testing.allocator);
    for (0..songs.items.len) |i| {
        print("{s}\n", .{songs.items[i].name});
    }
    for (0..songs.items.len) |i| {
        songs.items[i].deinit();
        std.testing.allocator.destroy(songs.items[i]);
    }
}

test "read library" {
    var file: fs.File = undefined;
    if (fs.cwd().openFile("/home/tal/Documents/Obsidian Vault/music/music.md", .{ .mode = .read_only })) |openedFile| {
        file = openedFile;
    } else |err| {
        print("Couldn't open music source file\n{}\n", .{err});
        return;
    }
    defer file.close();

    var songs: ArrayList(*Song) = ArrayList(*Song).init(std.testing.allocator);
    defer songs.deinit();
    try readLibrary(&songs, &file, std.testing.allocator);

    for (0..songs.items.len) |i| {
        songs.items[i].deinit();
        std.testing.allocator.destroy(songs.items[i]);
    }
}

const std = @import("std");
const Metadata = @import("metadata.zig").Metadata;
const Song = @import("song.zig").Song;
const ChildProcess = std.process.Child;
const print = std.debug.print;
const fs = std.fs;
const cwd = fs.cwd();
const ArrayList = std.ArrayList;
const String = ArrayList(u8);
const mem = std.mem;
const expect = std.testing.expect;
