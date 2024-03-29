#include <iostream>
#include <memory>

#include "error_handler.hpp"
#include "parser.hpp"
#include "resolver.hpp"
#include "scanner.hpp"
#include "treewalk_interpreter.hpp"
#include "utils.hpp"

namespace Jlox {

TreewalkInterpreter::TreewalkInterpreter()
    : errorHandler(std::make_unique<ErrorHandler>()),
      interpreter(Interpreter(*errorHandler.get())) {}

void TreewalkInterpreter::runFile(const char *path) {
    run(readFile(path));

    if (errorHandler->hadError) {
        std::exit(65);
    }
    if (errorHandler->hadRuntimeError) {
        std::exit(70);
    }
}

void TreewalkInterpreter::run(const std::string &source) {
    auto scanner = Scanner(source);
    auto tokens = std::vector<Token>();
    for (;;) {
        Token token = scanner.scanOneToken();
        if (token.type == TokenType::ERROR) {
            errorHandler->error(token.line, token.getLexemeString().data());
            break;
        }

        tokens.push_back(token);
        if (token.type == TokenType::EOF_) {
            break;
        }
    }
    if (errorHandler->hadError) {
        return;
    }

    auto parser = Parser(tokens, *errorHandler.get());
    auto statements = parser.parse();

    if (errorHandler->hadError) {
        return;
    }

    auto resolver = Resolver(interpreter, *errorHandler.get());
    resolver.resolve(statements);
    if (errorHandler->hadError) {
        return;
    }

    interpreter.interpret(statements);
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

} // namespace Jlox
