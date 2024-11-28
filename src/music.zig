const std = @import("std");
const ChildProcess = std.process.Child;
const print = std.debug.print;
const fs = std.fs;
const cwd = fs.cwd();
const ArrayList = std.ArrayList;
const String = ArrayList(u8);

pub fn update(file: *fs.File, allocator: std.mem.Allocator, dir: []const u8) !void {
    var url: String = String.init(allocator);
    var fileName: String = String.init(allocator);
    defer url.deinit();
    defer fileName.deinit();

    var downloaded = ArrayList(String).init(allocator);
    defer downloaded.deinit();
    var inFile = ArrayList(String).init(allocator);
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
        var name: String = String.init(allocator);
        try name.appendSlice(getName(line));

        var temp: String = String.init(allocator);
        try temp.appendSlice(name.items);
        try inFile.append(temp);

        // Check if is downloaded
        const URL = getURL(line);
        if (try isDownloaded(name.items, allocator)) {
            print("{s} is already downloaded!\n", .{name.items});
        } else try downloadYT(URL, name.items, dir);
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
    for (0..inFile.items.len) |i| {
        inFile.items[i].deinit();
    }
    for (0..deleteQueue.items.len) |i| {
        deleteQueue.items[i].deinit();
    }
}

fn isToDelete(fileName: *String, lines: *ArrayList(String)) bool {
    var a: usize = 0;
    for (0..lines.items.len) |i| {
        a = i;
        if (std.mem.eql(u8, lines.items[i].items, fileName.items)) {
            return false;
        }
    }
    return true;
}

fn isDownloaded(name: []const u8, allocator: std.mem.Allocator) !bool {
    var musicFolder: fs.Dir = try cwd.openDir("/home/tal/Music", .{ .iterate = true });
    defer musicFolder.close();

    var walker = try musicFolder.walk(allocator);
    defer walker.deinit();
    while (try walker.next()) |file| {
        if (std.mem.eql(u8, name, file.basename[0 .. file.basename.len - 4]))
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

fn getName(line: []const u8) []const u8 {
    var i: usize = 0;
    while (line[i] != ']') {
        i += 1;
    }
    return line[1..i];
}

fn getURL(line: []const u8) []const u8 {
    var i: usize = 0;

    while (line[i] != '(') {
        i += 1;
    }

    var j: usize = i;
    while (line[j] != ')') {
        j += 1;
    }

    return line[i + 1 .. j];
}

fn delete(fileNames: ArrayList(String), allocator: std.mem.Allocator) !void {
    print("Are you sure you want to delete:\n", .{});
    for (0..fileNames.items.len) |i| {
        print("{s}\n", .{fileNames.items[i].items});
    }

    var buff: [10]u8 = undefined;
    const stdin = std.io.getStdIn().reader();
    const input = try stdin.readUntilDelimiter(buff[0..], '\n');

    if (!std.mem.eql(u8, input, "y"))
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

const expect = std.testing.expect;
test "Get File Name from Line" {
    const line = "[Metallica - Master of Puppets](https://www.youtube.com/watch?v=mOOzcgDLCbQ&list=WL&index=58)";
    const name = getName(line);

    try expect(std.mem.eql(u8, name, "Metallica - Master of Puppets"));
}

test "Get URL from Line" {
    const line = "[Metallica - Master of Puppets](https://www.youtube.com/watch?v=mOOzcgDLCbQ&list=WL&index=58)";
    const URL = getURL(line);

    try expect(std.mem.eql(u8, URL, "https://www.youtube.com/watch?v=mOOzcgDLCbQ&list=WL&index=58"));
}

test "checkIfDownloaded" {
    const line = "[Metallica - Master of Puppets](https://www.youtube.com/watch?v=mOOzcgDLCbQ&list=WL&index=58)";
    const name = getName(line);

    try expect(try isDownloaded(name, std.testing.allocator));
}

test "update()" {
    var file: fs.File = try cwd.openFile("/home/tal/Documents/Obsidian Vault/music/music.md", .{ .mode = .read_only });
    defer file.close();

    try update(&file, std.testing.allocator);
}
