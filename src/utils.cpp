#include <iostream>
#include <utils.hpp>

void confirmUserInput(std::function<void(void)> callback) {
	std::cout << "y/N\n";

	std::string answer;
	std::getline(std::cin, answer);

	if (answer == "y" || answer == "Y") {
		callback();
	}
}
