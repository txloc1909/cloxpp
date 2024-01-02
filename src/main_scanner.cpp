#include <iostream>
#include <memory>
#include <string>

#include "scanner.hpp"
#include "error_handler.hpp"
#include "token.hpp"
#include "utils.hpp"


void scan(std::string source) {
    auto scanner = Scanner(source, std::make_shared<ErrorHandler>());
    for (const Token& t : scanner.scanTokens()) {
        std::cout << t << "\n";
    }
}

void scanFile(const char* path) {
    scan(readFile(path));
}

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
