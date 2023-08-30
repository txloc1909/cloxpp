#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>

#include "errorhandler.hpp"
#include "scanner.hpp"
#include "treewalkinterpreter.hpp"


TreewalkInterpreter::TreewalkInterpreter() {
    this->errorHandler = std::make_shared<ErrorHandler>();
}


void TreewalkInterpreter::runFile(const char* path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) {
        throw std::ios_base::failure("File does not exist!");
    }
    std::string source(std::istreambuf_iterator<char>{ifs}, {});
    run(source);

    if (errorHandler->hadError) {
        std::exit(65);
    }
}

void TreewalkInterpreter::run(std::string source) {
    auto scanner = Scanner(source, this->errorHandler);
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
        errorHandler->hadError = false;
    }
}
