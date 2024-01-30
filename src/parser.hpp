#ifndef CLOXPP_PARSER_H
#define CLOXPP_PARSER_H

#include <cstddef>
#include <initializer_list>
#include <vector>

#include "error_handler.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

class Parser {
private:
    ErrorHandler &handler;
    std::vector<Token> tokens;
    std::size_t current = 0;

    class ParserError : std::exception {};

    Expr::ExprPtr expression();
    Expr::ExprPtr comparison();
    Expr::ExprPtr term();
    Expr::ExprPtr factor();
    Expr::ExprPtr unary();
    Expr::ExprPtr primary();

    Stmt::StmtPtr declaration();
    Stmt::StmtPtr varDeclaration();
    Stmt::StmtPtr statement();
    Stmt::StmtPtr expressionStmt();
    Stmt::StmtPtr printStmt();

    void synchronize();
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    bool isAtEnd();
    Token advance();
    Token peek();
    Token previous();
    Token consume(TokenType type, const char *message);
    ParserError error(Token token, const char *message);

public:
    Parser(const std::vector<Token> &tokens, ErrorHandler &handler);
    std::vector<Stmt::StmtPtr> parse();
};

#endif // !CLOXPP_PARSER_H
