#ifndef CLOXPP_SCANNER_H
#define CLOXPP_SCANNER_H

#include <optional>
#include <string>
#include <vector>

#include "error_handler.hpp"
#include "token.hpp"

class Scanner {
private:
    ErrorHandler &handler;
    const std::string &source;
    const char *start;
    const char *current;
    std::size_t line;

    std::optional<Token> scanOneToken();
    Token createToken(TokenType type);
    Token createToken(TokenType type, std::string_view lexeme);
    std::optional<Token> consumeString();
    Token consumeNumber();
    Token consumeIdentifier();
    bool isAtEnd();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();

public:
    Scanner(const std::string &source, ErrorHandler &handler);
    std::vector<Token> scanTokens();
};

#endif // !CLOXPP_SCANNER_H
