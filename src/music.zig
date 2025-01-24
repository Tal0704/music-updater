const std = @import("std");
const Song = @import("song.zig").Song;
const Album = @import("album.zig").Album;
const ChildProcess = std.process.Child;
const print = std.debug.print;
const fs = std.fs;
const cwd = fs.cwd();
const ArrayList = std.ArrayList;
const String = ArrayList(u8);
const mem = std.mem;
const expect = std.testing.expect;

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
        var album = Album.init(allocator);
        defer album.deinit();
        song.* = Song.init(allocator, &album);

        try song.name.appendSlice(file.path[0 .. file.path.len - 4]);
        _ = songs;
        _ = musicPath;
    }
}

fn readLibrary(songs: *ArrayList(*Song), file: *fs.File, allocator: std.mem.Allocator) !void {
    var buffReader = std.io.bufferedReader(file.reader());
    var inStream = buffReader.reader();
    var buf: [1024]u8 = undefined;

    while (try inStream.readUntilDelimiterOrEof(&buf, '\n')) |line| {
        if (line.len == 0 or line[0] == '-') continue;

        var song: *Song = try allocator.create(Song);
        var album = Album.init(allocator);
        defer album.deinit();
        song.* = Song.init(allocator, &album);

        if (getName(line)) |name| {
            try song.name.appendSlice(name);
        } else {
            song.deinit();
            allocator.destroy(song);
            continue;
        }

        try songs.append(song);
    }
}

fn isDeleteSong(songs: *ArrayList(*Song), song: *Song) bool {
    for (songs.items) |currentSong| {
        if (mem.eql(u8, song.name.items, currentSong.name.items))
            return false;
    }

    return true;
}

fn deleteSongs(songs: *ArrayList(*Song)) !void {
    print("Are you sure you want to delete:\n", .{});
    print("y/N\n", .{});

    var buff: [10]u8 = undefined;
    const stdin = std.io.getStdIn().reader();
    const input = try stdin.readUntilDelimiter(buff[0..], '\n');

    if (!mem.eql(u8, input, "y")) {
        print("Not deleteing!\n", .{});
        return;
    }
    _ = songs;
}

fn downloadSongs(songs: *ArrayList(*Song), dir: []const u8) void {
    _ = songs;
    _ = dir;
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
    if (!mem.eql(u8, line[0..2], "##"))
        return null;
    var i: usize = 4;
    while (line[i] != ']') {
        i += 1;
    }
    return line[4..i];
}

fn getYear(line: []const u8) ?[]const u8 {
    if (!mem.eql(u8, line[0..2], "##"))
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
        print("{s}\n", .{songs.items[i].name.items});
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
