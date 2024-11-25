const std = @import("std");
const ChildProcess = std.process.Child;
const print = std.debug.print;
const fs = std.fs;
const cwd = fs.cwd();
const ArrayList = std.ArrayList;
const String = ArrayList(u8);
const PageAllocator = std.heap.page_allocator;

pub fn main() anyerror!void {
    var file: fs.File = try cwd.openFile("/home/tal/Documents/Obsidian Vault/music/music.md", .{ .mode = .read_only });
    defer file.close();
    var url: String = String.init(PageAllocator);
    defer url.deinit();
    var fileName: String = String.init(PageAllocator);
    defer fileName.deinit();

    var buffReader = std.io.bufferedReader(file.reader());
    var inStream = buffReader.reader();
    var buf: [1024]u8 = undefined;
    while (try inStream.readUntilDelimiterOrEof(&buf, '\n')) |line| {
        const name = getName(line);
        const URL = getURL(line);

        if (try isDownloaded(name)) {
            print("{s} is already downloaded!\n", .{name});
        } else try downloadYT(URL, name);
    }
}

fn isDownloaded(name: []const u8) !bool {
    var musicFolder: fs.Dir = try cwd.openDir("/home/tal/Music", .{ .iterate = true });
    defer musicFolder.close();

    var walker = try musicFolder.walk(PageAllocator);
    while (try walker.next()) |file| {
        if (std.mem.eql(u8, name, file.basename[0 .. file.basename.len - 4]))
            return true;
    }
    return false;
}

fn downloadYT(URL: []const u8, name: []const u8) !void {
    const result = try ChildProcess.run(.{
        .allocator = std.heap.page_allocator,
        .argv = &[_][]const u8{
            "yt-dlp",
            "-x",
            "--audio-format",
            "mp3",
            "-P",
            "/home/tal/Music/",
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

    try expect(try isDownloaded(name));
}
