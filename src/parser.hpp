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
    std::vector<Token> tokens_;
    std::size_t current;
    ErrorHandler &handler_;

    class ParserError : std::exception {};

    Expr::ExprPtr expression();
    Expr::ExprPtr comparison();
    Expr::ExprPtr term();
    Expr::ExprPtr factor();
    Expr::ExprPtr unary();
    Expr::ExprPtr primary();

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
    Parser(std::vector<Token> token, ErrorHandler &handler);
    std::vector<Stmt::StmtPtr> parse();
};

#endif // !CLOXPP_PARSER_H
