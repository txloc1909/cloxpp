#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <string>

#include "treewalkinterpreter.hpp"

void TreewalkInterpreter::runFile(const char* path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) {
        throw std::ios_base::failure("File does not exist!");
    }
    std::string source(std::istreambuf_iterator<char>{ifs}, {});
    run(source);

    if (this->hadError) {
        std::exit(65);
    }
}

void TreewalkInterpreter::run(std::string source) {
    std::cout << source << std::endl;
}

void TreewalkInterpreter::runPrompt() {
    std::string line;
    while(std::cin) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        run(line);
        this->hadError = false;
    }
}
