#include <iostream>
#include <string>

#include "error_handler.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include "utils.hpp"

void scan(const std::string &source) {
    auto errorHandler = ErrorHandler();
    auto scanner = Scanner(source);

    for (;;) {
        Token token = scanner.scanOneToken();
        if (token.type == TokenType::ERROR) {
            errorHandler.error(token.line, token.getLexemeString().data());
            break;
        } else if (token.type == TokenType::EOF_) {
            break;
        } else {
            std::cout << token << "\n";
        }
    }
}

void scanFile(const char *path) { scan(readFile(path)); }

void scanREPL() {
    std::string line;
    while (std::cin) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        scan(line);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [script]";
        std::exit(64);
    } else if (argc == 2) {
        scanFile(argv[1]);
    } else {
        scanREPL();
    }
}
