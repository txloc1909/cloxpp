#include <initializer_list>
#include <memory>
#include <vector>

#include "expr.hpp"
#include "parser.hpp"
#include "token.hpp"

Parser::Parser(const std::vector<Token> &tokens, ErrorHandler &handler)
    : handler_(handler), tokens_(tokens) {}

std::vector<Stmt::StmtPtr> Parser::parse() {
    auto statements = std::vector<Stmt::StmtPtr>();
    while (!isAtEnd()) {
        statements.push_back(statement());
    }

    return statements;
}

Stmt::StmtPtr Parser::statement() {
    if (match(TokenType::PRINT)) {
        return printStmt();
    }

    return expressionStmt();
}

Stmt::StmtPtr Parser::expressionStmt() {
    Expr::ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<Stmt::Expr>(std::move(value));
}

Stmt::StmtPtr Parser::printStmt() {
    Expr::ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<Stmt::Print>(std::move(value));
}

Expr::ExprPtr Parser::expression() {
    auto expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        auto op = previous();
        auto right = comparison();
        expr = std::make_unique<Expr::Binary>(std::move(expr), std::move(right),
                                              op);
    }

    return expr;
}

Expr::ExprPtr Parser::comparison() {
    auto expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                  TokenType::LESS_EQUAL})) {
        auto op = previous();
        auto right = term();
        expr = std::make_unique<Expr::Binary>(std::move(expr), std::move(right),
                                              op);
    }

    return expr;
}

Expr::ExprPtr Parser::term() {
    auto expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        auto op = previous();
        auto right = factor();
        expr = std::make_unique<Expr::Binary>(std::move(expr), std::move(right),
                                              op);
    }

    return expr;
}

Expr::ExprPtr Parser::factor() {
    auto expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        auto op = previous();
        auto right = factor();
        expr = std::make_unique<Expr::Binary>(std::move(expr), std::move(right),
                                              op);
    }

    return expr;
}

Expr::ExprPtr Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        auto op = previous();
        auto right = unary();
        return std::make_unique<Expr::Unary>(op, std::move(right));
    }

    return primary();
}

Expr::ExprPtr Parser::primary() {
    if (match(TokenType::FALSE))
        return std::make_unique<Expr::Literal>(false);
    if (match(TokenType::TRUE))
        return std::make_unique<Expr::Literal>(true);
    if (match(TokenType::NIL))
        return std::make_unique<Expr::Literal>(std::monostate{});

    if (match({TokenType::NUMBER, TokenType::STRING}))
        return std::make_unique<Expr::Literal>(previous().literal.value());

    if (match(TokenType::LEFT_PAREN)) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression");
        return std::make_unique<Expr::Grouping>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON)
            return;

        switch (peek().type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        }

        advance();
    }
}

auto Parser::error(Token token, const char *message) -> Parser::ParserError {
    handler_.error(token, message);
    return ParserError();
}

Token Parser::consume(TokenType type, const char *message) {
    if (check(type))
        return advance();

    throw error(peek(), message);
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }

    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) {
        return false;
    }

    return peek().type == type;
}

bool Parser::isAtEnd() { return peek().type == TokenType::EOF_; }

Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }

    return previous();
}

Token Parser::peek() { return tokens_.at(current); }

Token Parser::previous() { return tokens_.at(current - 1); }
