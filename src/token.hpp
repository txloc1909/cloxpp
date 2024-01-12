#ifndef CLOXPP_TOKEN_H
#define CLOXPP_TOKEN_H

#include <ostream>

#include "literal.hpp"

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

    EOF_,
};
std::ostream &operator<<(std::ostream &os, const TokenType type);

typedef struct Token {
    const TokenType type;
    const std::string lexeme;
    const Literal literal;
    const int line;

    Token(TokenType type_, std::string lexeme_, Literal literal_, int line_)
        : type(type_), lexeme(lexeme_), literal(literal_), line(line_){};
} Token;

std::ostream &operator<<(std::ostream &os, const Token &token);

#endif // !CLOXPP_TOKEN_H
