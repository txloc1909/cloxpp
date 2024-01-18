#include <iostream>
#include <memory>

#include "ast_stringifier.hpp"
#include "error_handler.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "treewalk_interpreter.hpp"
#include "utils.hpp"

TreewalkInterpreter::TreewalkInterpreter() {
    this->errorHandler = std::make_shared<ErrorHandler>();
}

void TreewalkInterpreter::runFile(const char *path) {
    run(readFile(path));

    if (errorHandler->hadError) {
        std::exit(65);
    }
}

void TreewalkInterpreter::run(std::string source) {
    auto scanner = Scanner(source, this->errorHandler);
    auto parser = Parser(scanner.scanTokens(), this->errorHandler.get());
    auto expr = parser.parse();

    if (this->errorHandler->hadError) {
        return;
    }

    std::cout << ASTStringifier().visit(*expr) << "\n";
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
