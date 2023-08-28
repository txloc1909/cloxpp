#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <string>

#include "scanner.hpp"
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
    auto scanner = Scanner(source);
    for(const auto& t: scanner.scanTokens()) {
        std::cout << t << "\n";
    }
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

void TreewalkInterpreter::report(int line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message;
    this->hadError = true;
}

void TreewalkInterpreter::error(int line, const std::string& message) {
    this->report(line, "", message);
}
