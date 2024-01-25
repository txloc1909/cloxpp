#include <iostream>
#include <memory>

#include "error_handler.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "treewalk_interpreter.hpp"
#include "utils.hpp"

TreewalkInterpreter::TreewalkInterpreter() {
    errorHandler = std::make_unique<ErrorHandler>();
    interpreter = Interpreter(this->errorHandler.get());
}

void TreewalkInterpreter::runFile(const char *path) {
    run(readFile(path));

    if (errorHandler->hadError) {
        std::exit(65);
    }
    if (errorHandler->hadRuntimeError) {
        std::exit(70);
    }
}

void TreewalkInterpreter::run(std::string source) {
    auto scanner = Scanner(source, *errorHandler.get());
    auto parser = Parser(scanner.scanTokens(), *errorHandler.get());
    auto expr = parser.parse();

    if (errorHandler->hadError) {
        return;
    }

    interpreter.interpret(std::move(expr));
}

void TreewalkInterpreter::runPrompt() {
    std::string line;
    while (std::cin) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        run(line);
        errorHandler->hadError = false;
    }
}
