#include <argparse/argparse.hpp>
#include <program.hpp>

#ifndef NDEBUG
int main() {
	Program program("/home/tal/Music",
	                "/home/tal/Documents/notes/music/music.md");
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
