#include <exec.hpp>

std::string exec(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    // Open the command for reading
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    // Read the output a line at a time - output it
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

