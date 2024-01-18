#ifndef CLOXPP_PARSER_H
#define CLOXPP_PARSER_H

#include "expr.hpp"
#include "token.hpp"
#include <cstddef>
#include <initializer_list>
#include <vector>

class Parser {
private:
    std::vector<Token> tokens_;
    std::size_t current;

    Expr::ExprPtr expression();
    Expr::ExprPtr comparison();
    Expr::ExprPtr term();
    Expr::ExprPtr factor();
    Expr::ExprPtr unary();
    Expr::ExprPtr primary();

    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    bool isAtEnd();
    Token advance();
    Token peek();
    Token previous();
    Token consume(TokenType type, const char *message);

public:
    Parser(std::vector<Token> token);
};

#endif // !CLOXPP_PARSER_H
