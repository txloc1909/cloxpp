#ifndef CLOXPP_TOKEN_H
#define CLOXPP_TOKEN_H

#include <ostream>

enum class TokenType {
    // Single-character token
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One-or-two-character token
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords
    AND,
    CLASS,
    ELSE,
    FALSE,
    FOR,
    FUN,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    ERROR,
    EOF_,
};
std::ostream &operator<<(std::ostream &os, const TokenType type);

struct Token {
    const TokenType type;
    const std::string_view lexeme;
    const int line;

    Token() : type(TokenType::EOF_), lexeme("\0"), line(1) {}
    Token(TokenType type_, std::string_view lexeme_, int line_)
        : type(type_), lexeme(std::move(lexeme_)), line(line_){};

    std::string getLexemeString() const;
};

std::ostream &operator<<(std::ostream &os, const Token &token);

#endif // !CLOXPP_TOKEN_H
