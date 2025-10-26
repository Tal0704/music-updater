#include <argparse/argparse.hpp>
#include <library.hpp>
#include <program.hpp>

#ifndef NDEBUG
int main() {
	Library lib;
	Album::Ptr album = std::make_shared<Album>("Master of puppets");
	Song song("Orion", album);
	lib.addSong(song, *album);
	song.setName("Master");
	lib.addSong(song, *album);
	Album::Ptr a = std::make_shared<Album>("Meteora");
	Song s("Don't Stay", a);
	lib.addSong(s, *a);

	for (auto it = lib.begin(); it != lib.end(); ++it) {
		std::cout << **it << "\n";
	}

	return 0;

	Program program("/home/tal/Music/M",
	                "/home/tal/Documents/notes/music/musicTemp.md");
	program.run();
	return 0;
}
#else
#include <iostream>
int main(int argc, char **argv) {
	argparse::ArgumentParser parser(argv[0]);
	parser.add_argument("musicFolder").help("Path to music directory to edit");
	parser.add_argument("library").help(
	    "Path to music library to edit according to");

	try {
		parser.parse_args(argc, argv);
	} catch (const std::exception &err) {
		std::cerr << err.what() << "\n";
		std::cerr << parser << "\n";
		return -1;
	}

	Program program(parser.get("musicFolder"), parser.get("library"));
	program.run();
	return 0;
}
#endif
