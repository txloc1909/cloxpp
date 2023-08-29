#ifndef CLOXPP_SCANNER_H
#define CLOXPP_SCANNER_H

#include <memory>
#include <ostream>
#include <vector>
#include <variant>
#include <string>
#include <optional>

enum class TokenType {
    // Single-character token
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS,
    SEMICOLON, SLASH, STAR,

    // One-or-two-character token
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    AND, CLASS, ELSE, FALSE,
    FOR, FUN, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS,
    TRUE, VAR, WHILE,

    EOF_,
};
std::ostream& operator<<(std::ostream &os, const TokenType type);

using Literal = std::optional<std::variant<std::string, double, bool>>;
std::ostream& operator<<(std::ostream &os, const Literal& literal);

typedef struct Token {
    const TokenType type;
    const std::string lexeme;
    Literal literal;
    const int line;

    Token(TokenType type_, std::string lexeme_, Literal literal_, int line_)
        : type(type_), lexeme(lexeme_), literal(literal_), line(line_) {};
    std::string toString();
} Token;
std::ostream& operator<<(std::ostream &os, const Token &token);

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
