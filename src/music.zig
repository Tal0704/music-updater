pub fn update(file: *fs.File, allocator: mem.Allocator, dir: *fs.Dir) !void {
    var songs: ArrayList(*Song) = ArrayList(*Song).init(allocator);
    defer songs.deinit();

    try readLibrary(&songs, file, allocator);
    try readFiles(&songs, dir, allocator);

    print("Files To delete:\n", .{});
    for (songs.items) |song| {
        if (song.isToDelete) {
            song.print();
            print("\n\n", .{});
        }
    }

    print("Files to keep:\n", .{});
    for (songs.items) |song| {
        if (!song.isToDelete) {
            song.print();
            print("\n\n", .{});
        }
    }
}

fn readFiles(songs: *ArrayList(*Song), musicFolder: *fs.Dir, allocator: mem.Allocator) !void {
    var walker = try musicFolder.walk(allocator);
    defer walker.deinit();
    while (try walker.next()) |line| {
        var song: *Song = try allocator.create(Song);
        song.* = Song.init(allocator);
        song.name = try song.allocator.alloc(u8, line.path.len - 4);

        mem.copyForwards(u8, song.name.?, line.path[0 .. line.path.len - 4]);

        if (song.name) |_|
            song.isToDelete = isDeleteSong(songs, song);

        if (song.isToDelete) {
            try songs.append(song);
        } else {
            song.deinit();
            allocator.destroy(song);
        }
    }
}

fn isDeleteSong(songs: *ArrayList(*Song), song: *Song) bool {
    for (songs.items) |currentSong| {
        if (mem.eql(u8, song.name.?, currentSong.name.?))
            return false;
    }

    return true;
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
            mem.copyForwards(u8, song.name.?, name);
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

// fn fixSpaces(metadata: *Metadata) !void {
//     var i: usize = 0;
//     while (i < metadata.album.items.len) {
//         if (metadata.album.items[i] == ' ') {
//             try metadata.album.insert(i, '\\');
//             i += 1;
//         }
//         i += 1;
//     }
//     i = 0;
//     while (i < metadata.year.items.len) {
//         if (metadata.year.items[i] == ' ') {
//             try metadata.year.insert(i, '\\');
//             i += 1;
//         }
//         i += 1;
//     }
//     i = 0;
//     while (i < metadata.artist.items.len) {
//         if (metadata.artist.items[i] == ' ') {
//             try metadata.artist.insert(i, '\\');
//             i += 1;
//         }
//         i += 1;
//     }
// }

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
    if (getArtist(line)) |a|
        try expect(mem.eql(u8, a, "Metallica"));
}

// test "fix spaces" {
//     var metadata = Metadata{
//         .year = undefined,
//         .album = undefined,
//         .artist = undefined,
//     };
//     defer metadata.deinit();
//     metadata.artist = "Metallica";
//     metadata.year = "1989"[0..];
//     metadata.album = "Master of Puppets"[0..];

//     try fixSpaces(&metadata);

//     try expect(mem.eql(u8, metadata.album.items, "Master\\ of\\ Puppets"));
//     try expect(mem.eql(u8, metadata.year.items, "1989"));
//     try expect(mem.eql(u8, metadata.artist.items, "Metallica"));
// }

test "read files" {
    var musicFolder: fs.Dir = try cwd.openDir("/home/tal/Music", .{ .iterate = true });
    defer musicFolder.close();

    var songs: ArrayList(*Song) = ArrayList(*Song).init(std.testing.allocator);
    defer songs.deinit();
    try readFiles(&songs, &musicFolder, std.testing.allocator);
    for (0..songs.items.len) |i| {
        if (songs.items[i].name) |name|
            print("{s}\n", .{name});
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
