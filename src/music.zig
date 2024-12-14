pub fn update(file: *fs.File, allocator: mem.Allocator, dir: []const u8) !void {
    var songs: ArrayList(Song) = ArrayList(Song).init(allocator);
    defer songs.deinit();

    var downloaded = ArrayList(String).init(allocator);
    defer downloaded.deinit();
    var inFile = ArrayList([]const u8).init(allocator);
    defer inFile.deinit();
    var deleteQueue = ArrayList(String).init(allocator);
    defer deleteQueue.deinit();

    var musicFolder: fs.Dir = try cwd.openDir("/home/tal/Music", .{ .iterate = true });
    defer musicFolder.close();

    // Read in all files
    var walker = try musicFolder.walk(allocator);
    defer walker.deinit();
    while (try walker.next()) |line| {
        var baseNameSlice: String = try String.initCapacity(allocator, line.basename.len - 3);
        try baseNameSlice.appendSlice(line.basename[0 .. line.basename.len - 4]);

        try downloaded.append(baseNameSlice);
    }

    // Read in all lines + download;
    var buffReader = std.io.bufferedReader(file.reader());
    var inStream = buffReader.reader();
    var buf: [1024]u8 = undefined;
    while (try inStream.readUntilDelimiterOrEof(&buf, '\n')) |line| {
        if (line.len == 0 or line[0] == '-') {
            continue;
        }
        var song: Song = undefined;
        if (getName(line)) |n|
            song.name = n[0..];

        if (try isDownloaded(song.name, allocator)) {
            print("{s} is already downloaded!\n", .{song.name});
        } else if (getURL(line)) |u| {
            print("Downloading {s}!\n", .{song.name});
            song.URL = u[0..];
            try song.download(dir);
        }

        try songs.append(song);
    }

    // get all files that are not in the music.md file
    for (0..downloaded.items.len) |i| {
        if (isToDelete(&downloaded.items[i], &inFile)) {
            var temp: String = String.init(allocator);
            try temp.appendSlice(downloaded.items[i].items);
            try deleteQueue.append(temp);
        }
    }
    // Delete them
    if (deleteQueue.items.len > 0)
        try delete(deleteQueue, allocator);

    for (0..downloaded.items.len) |i| {
        downloaded.items[i].deinit();
    }
    for (0..deleteQueue.items.len) |i| {
        deleteQueue.items[i].deinit();
    }
}

fn isToDelete(fileName: *String, lines: *ArrayList([]const u8)) bool {
    var a: usize = 0;
    for (0..lines.items.len) |i| {
        a = i;
        if (mem.eql(u8, lines.items[i], fileName.items)) {
            return false;
        }
    }
    return true;
}

fn isDownloaded(name: []const u8, allocator: mem.Allocator) !bool {
    var musicFolder: fs.Dir = try cwd.openDir("/home/tal/Music", .{ .iterate = true });
    defer musicFolder.close();

    var walker = try musicFolder.walk(allocator);
    defer walker.deinit();
    while (try walker.next()) |file| {
        if (mem.eql(u8, name, file.basename[0 .. file.basename.len - 4]))
            return true;
    }
    return false;
}

fn downloadYT(URL: []const u8, name: []const u8, dir: []const u8) !void {
    const result = try ChildProcess.run(.{
        .allocator = std.heap.page_allocator,
        .argv = &[_][]const u8{
            "yt-dlp",
            "-x",
            "--audio-format",
            "mp3",
            "-P",
            dir,
            "-o",
            name,
            URL,
        },
    });
    std.debug.print("{s}\n", .{result.stdout});
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

fn delete(fileNames: ArrayList(String), allocator: mem.Allocator) !void {
    print("Are you sure you want to delete:\n", .{});
    for (0..fileNames.items.len) |i| {
        print("{s}\n", .{fileNames.items[i].items});
    }

    var buff: [10]u8 = undefined;
    const stdin = std.io.getStdIn().reader();
    const input = try stdin.readUntilDelimiter(buff[0..], '\n');

    if (!mem.eql(u8, input, "y"))
        return;

    for (0..fileNames.items.len) |i| {
        var temp: String = String.init(allocator);
        defer temp.deinit();

        try temp.appendSlice("/home/tal/Music/");
        try temp.appendSlice(fileNames.items[i].items);
        try temp.appendSlice(".mp3");

        try fs.deleteFileAbsolute(temp.items);
        print("Succcessfully deleted '{s}'.mp3!\n", .{temp.items});
    }
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

fn fixSpaces(metadata: *Metadata) !void {
    var i: usize = 0;
    while (i < metadata.album.items.len) {
        if (metadata.album.items[i] == ' ') {
            try metadata.album.insert(i, '\\');
            i += 1;
        }
        i += 1;
    }
    i = 0;
    while (i < metadata.year.items.len) {
        if (metadata.year.items[i] == ' ') {
            try metadata.year.insert(i, '\\');
            i += 1;
        }
        i += 1;
    }
    i = 0;
    while (i < metadata.artist.items.len) {
        if (metadata.artist.items[i] == ' ') {
            try metadata.artist.insert(i, '\\');
            i += 1;
        }
        i += 1;
    }
}

const expect = std.testing.expect;
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

test "checkIfDownloaded" {
    const line = "[Master of Puppets](https://www.youtube.com/watch?v=mOOzcgDLCbQ&list=WL&index=58)";
    const name = getName(line);

    if (name) |n|
        try expect(try isDownloaded(n, std.testing.allocator));
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
    const artist = getArtist(line);
    if (artist) |a|
        try expect(mem.eql(u8, a, "Metallica"));
}

test "fix spaces" {
    var metadata = Metadata{
        .year = String.init(std.testing.allocator),
        .album = String.init(std.testing.allocator),
        .artist = String.init(std.testing.allocator),
    };
    defer metadata.deinit();
    try metadata.artist.appendSlice("Metallica");
    try metadata.year.appendSlice("1989");
    try metadata.album.appendSlice("Master of Puppets");

    try fixSpaces(&metadata);

    try expect(mem.eql(u8, metadata.album.items, "Master\\ of\\ Puppets"));
    try expect(mem.eql(u8, metadata.year.items, "1989"));
    try expect(mem.eql(u8, metadata.artist.items, "Metallica"));
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
