#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <string>

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

void runFile(const char* path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) {
        throw std::ios_base::failure("File does not exist!");
    }
    std::string source(std::istreambuf_iterator<char>{ifs}, {});
    run(source);
}

void run(std::string source) {
    std::cout << source << std::endl;
}

void runPrompt() {
    std::string line;
    while(std::cin) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        run(line);
    }
}
