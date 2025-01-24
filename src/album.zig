const Song = @import("song.zig").Song;
const std = @import("std");
const ChildProcess = std.process.Child;
const ArrayList = std.ArrayList;
const String = ArrayList(u8);
const mem = std.mem;
const http = std.http;
const json = std.json;

pub const Album = struct {
    name: String,
    year: String,
    imageURL: String,
    artist: String,
    songs: ArrayList(Song),
    allocator: mem.Allocator,

    pub fn init(allocator: mem.Allocator) Album {
        return @This(){
            .allocator = allocator,
            .name = String.init(allocator),
            .year = String.init(allocator),
            .imageURL = String.init(allocator),
            .artist = String.init(allocator),
            .songs = ArrayList(Song).init(allocator),
        };
    }

    pub fn deinit(self: @This()) void {
        self.name.deinit();
        self.year.deinit();
        self.imageURL.deinit();
        self.artist.deinit();
        self.songs.deinit();
    }

    pub fn download() !void {}
    fn downloadImage() !void {}

    fn getMetadata(self: @This()) !void {
        _ = self;
    }
};

// test "Album::getMetadata" {
//     var album = Album.init(std.testing.allocator);
//     defer album.deinit();
//     try album.name.insertSlice(0, "master%20of%20puppets");
//     try album.artist.insertSlice(0, "Metallica");

//     // var song: Song = Song.init();
//     // defer song.deinit();
//     // song.name.insertSlice(0, "master of puppets");
//     // album.songs.append(song);

//     try album.getMetadata();
// }

test "HTTPS request" {
    const print = std.debug.print;
    const alloc = std.testing.allocator;

    const baseURL = "https://musicbrainz.org/ws/2/release/?query=artist:";
    const query = std.fmt.allocPrint(alloc, "{s}{s}%20AND%20release:{s}&fmt=json", .{ baseURL, "metallica", "master%20of%20puppets" }) catch return;
    defer alloc.free(query);

    const uri = try std.Uri.parse(query);

    print("{any}\n", .{uri});
    var client: std.http.Client = std.http.Client{ .allocator = alloc };
    defer client.deinit();

    const server_header_buffer: []u8 = try alloc.alloc(u8, 1024 * 8);
    defer alloc.free(server_header_buffer);

    // Make the connection to the server.
    var req = try client.open(.GET, uri, .{
        .server_header_buffer = server_header_buffer,
    });
    defer req.deinit();

    try req.send();
    try req.finish();
    try req.wait();

    const body = try req.reader().readAllAlloc(alloc, 1024 * 80);
    defer alloc.free(body);

    const parsed = try json.parseFromSlice(String, alloc, body, .{});
    defer parsed.deinit();

    const bo = parsed.value;
    print("year: {s}", bo.items);
}
