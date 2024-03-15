#ifndef CLOXPP_SCANNER_H
#define CLOXPP_SCANNER_H

#include <string>

#include "token.hpp"

class Scanner {
private:
    const std::string &source;
    const char *start;
    const char *current;
    std::size_t line;

    Token createToken(TokenType type);
    Token createToken(TokenType type, std::string_view lexeme);
    Token createErrorToken(const char *message);
    void skipWhitespace();
    TokenType checkKeyword(std::size_t start, std::size_t length,
                           const char *rest, TokenType type);
    TokenType identifierType();
    Token consumeString();
    Token consumeNumber();
    Token consumeIdentifier();
    bool isAtEnd();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();

public:
    Scanner(const std::string &source);
    Token scanOneToken();
};

#endif // !CLOXPP_SCANNER_H
