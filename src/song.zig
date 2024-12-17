pub const Song = struct {
    name: []u8,
    URL: ?[]u8,
    path: ?[]u8,
    metadata: ?Metadata,
    isDownloaded: bool,
    isToDelete: bool,
    allocator: std.mem.Allocator,

    pub fn deinit(self: @This()) void {
        self.allocator.free(self.name);
        if (self.URL) |URL| {
            self.allocator.free(URL);
        }
        if (self.metadata) |metadata| {
            metadata.deinit();
        }
        if (self.path) |path| {
            self.allocator.free(path);
        }
    }

    pub fn init(allocator: mem.Allocator) Song {
        return @This(){
            .name = "",
            .URL = null,
            .metadata = null,
            .path = null,
            .isToDelete = false,
            .isDownloaded = false,
            .allocator = allocator,
        };
    }

    pub fn print(self: @This()) void {
        std.debug.print("name: {s}\nURL: {s}", .{
            self.name,
            if (self.URL != null) self.URL.? else "",
        });
    }

    pub fn download(self: @This(), dir: []const u8) !void {
        if (self.URL) |URL| {
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
                    URL,
                },
            });
            std.debug.print("{s}\n", .{result.stdout});
        } else return error.URLNotFound;
    }

    pub fn delete(self: @This()) !void {
        if (self.path) |path| {
            const buffer = try std.fmt.allocPrint(self.allocator, "{s}/{s}.mp3", .{ path, self.name });
            defer self.allocator.free(buffer);
            try fs.deleteFileAbsolute(buffer);
            gprint("Delete {s} succefully!\n", .{self.name});
        } else {
            return error.PathNotFound;
        }
    }
};

const std = @import("std");
const Metadata = @import("metadata.zig").Metadata;
const ChildProcess = std.process.Child;
const mem = std.mem;
const gprint = std.debug.print;
const fs = std.fs;
