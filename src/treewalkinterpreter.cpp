#include <iostream>
#include <memory>

#include "errorhandler.hpp"
#include "scanner.hpp"
#include "treewalkinterpreter.hpp"
#include "utils.hpp"

TreewalkInterpreter::TreewalkInterpreter() {
    this->errorHandler = std::make_shared<ErrorHandler>();
}

void TreewalkInterpreter::runFile(const char* path) {
    run(readFile(path));

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
