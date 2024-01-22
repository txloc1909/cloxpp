#include <iostream>

#include "error_handler.hpp"
#include "token.hpp"

void ErrorHandler::report(int line, const std::string &where,
                          const char *message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message
              << "\n";
    hadError = true;
}

void ErrorHandler::error(int line, const char *message) {
    report(line, "", message);
}

void ErrorHandler::error(Token token, const char *message) {
    if (token.type == TokenType::EOF_) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" + token.lexeme + "'", message);
    }
}

void ErrorHandler::runtimeError(const RuntimeError &error) {
    std::cerr << error.what()
              << "\n[line " + std::to_string(error.getToken().line) << "]\n";
    hadRuntimeError = true;
}
