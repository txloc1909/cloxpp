#ifndef CLOXPP_SCANNER_H
#define CLOXPP_SCANNER_H

#include <memory>
#include <ostream>
#include <vector>
#include <variant>
#include <string>
#include <optional>

#include "token.hpp"

class Scanner {
    private:
        std::unique_ptr<std::string> source;
        std::size_t start;
        std::size_t current;
        std::size_t line;

        std::optional<Token> scanOneToken();
        Token createToken(TokenType type);
        Token createToken(TokenType type, Literal literal);
        std::optional<Token> consumeString();
        Token consumeNumber();
        Token consumeIdentifier();
        bool isAtEnd();
        char advance();
        bool match(char expected);
        char peek();
        char peekNext();

    public:
        Scanner(std::string source_);
        std::vector<Token> scanTokens();
};

#endif // !CLOXPP_SCANNER_H
