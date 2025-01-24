const std = @import("std");
const Album = @import("album.zig").Album;
const ChildProcess = std.process.Child;
const mem = std.mem;
const gprint = std.debug.print;
const fs = std.fs;
const String = std.ArrayList(u8);

pub const Song = struct {
    name: String,
    father: *Album,

    pub fn deinit(self: @This()) void {
        self.name.deinit();
    }

    pub fn init(allocator: mem.Allocator, father: *Album) Song {
        return @This(){
            .name = String.init(allocator),
            .father = father,
        };
    }

    pub fn print(self: @This()) void {
        std.debug.print("name: {s}\nURL: {s}", .{
            self.name.items,
            if (self.URL != null) self.URL.?.items else "",
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
                    self.name.items,
                    URL.items,
                    "-N",
                    "15",
                    "-q",
                },
            });
            std.debug.print("{s}", .{result.stdout});
        } else return error.URLNotFound;
    }

    pub fn delete(self: @This()) !void {
        if (self.path) |path| {
            const buffer = try std.fmt.allocPrint(self.allocator, "{s}/{s}.mp3", .{ path.items, self.name.items });
            defer self.allocator.free(buffer);
            try fs.deleteFileAbsolute(buffer);
            gprint("Delete {s} succefully!\n", .{self.name.items});
        } else {
            return error.PathNotFound;
        }
    }
};
