pub const Song = struct {
    name: ?[]u8,
    URL: ?[]u8,
    metadata: ?Metadata,
    isToDelete: bool,
    allocator: std.mem.Allocator,

    pub fn deinit(self: @This()) void {
        if (self.name) |name| {
            self.allocator.free(name);
        }
        if (self.URL) |URL| {
            self.allocator.free(URL);
        }
        if (self.metadata) |metadata| {
            metadata.deinit();
        }
    }

    pub fn init(allocator: mem.Allocator) Song {
        return @This(){
            .name = null,
            .URL = null,
            .metadata = null,
            .isToDelete = false,
            .allocator = allocator,
        };
    }

    pub fn print(self: @This()) void {
        std.debug.print("name: {s}\nURL: {s}", .{
            if (self.name != null) self.name.? else "",
            if (self.URL != null) self.URL.? else "",
        });
    }

    pub fn download(self: @This(), dir: []const u8) !void {
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
                self.name,
                self.URL,
            },
        });
        std.debug.print("{s}\n", .{result.stdout});
    }
};

const std = @import("std");
const Metadata = @import("metadata.zig").Metadata;
const ChildProcess = std.process.Child;
const mem = std.mem;
const print = std.debug.print;
