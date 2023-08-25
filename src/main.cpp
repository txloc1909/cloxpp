#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void run(std::string source);
void runFile(const char* path);
void runPrompt();

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: jlox [script]";
        return 64;
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        runPrompt();
    }
}

std::string readFile(std::string path) {
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f) {
        throw std::ios_base::failure("File does not exist!");
    }
    const auto size = fs::file_size(path);
    std::string result(size, '\0');
    f.read(result.data(), size);
    return result;
}

void runFile(const char* path) {
    run(readFile(std::string(path)));
}

void run(std::string source) {
    std::cout << source;
}

void runPrompt() {

}
