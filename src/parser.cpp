#include <initializer_list>
#include <memory>
#include <optional>
#include <vector>

#include "expr.hpp"
#include "lox_function.hpp"
#include "parser.hpp"
#include "stmt.hpp"
#include "token.hpp"

Parser::Parser(const std::vector<Token> &tokens, ErrorHandler &handler)
    : handler(handler), tokens(tokens) {}

std::vector<Stmt::StmtPtr> Parser::parse() {
    auto statements = std::vector<Stmt::StmtPtr>();
    while (!isAtEnd()) {
        auto stmt = declaration();
        if (stmt) {
            statements.push_back(stmt);
        }
    }

    return statements;
}

Stmt::StmtPtr Parser::declaration() {
    try {
        if (match(TokenType::CLASS)) {
            return classDeclaration();
        }
        if (match(TokenType::FUN)) {
            return function(functionTypeToString(FunctionType::FUNCTION));
        }

        if (match(TokenType::VAR)) {
            return varDeclaration();
        }

        return statement();
    } catch (const ParserError &error) {
        synchronize();
        return nullptr;
    }
}

Stmt::StmtPtr Parser::classDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

    auto methods = std::vector<Stmt::FunctionPtr>();
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        methods.push_back(std::dynamic_pointer_cast<Stmt::Function const>(
            function(functionTypeToString(FunctionType::METHOD))));
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
    return std::make_shared<Stmt::Class>(name, std::move(methods));
}

Stmt::StmtPtr Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    Expr::ExprPtr initializer =
        match(TokenType::EQUAL) ? expression() : nullptr;

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<Stmt::Var>(name,
                                       initializer ? initializer : nullptr);
}

Stmt::StmtPtr Parser::function(const char *type) {
    const auto functionType = std::string(type);
    Token name =
        consume(TokenType::IDENTIFIER, "Expect " + functionType + " name.");
    consume(TokenType::LEFT_PAREN,
            "Expect '(' after " + functionType + " name.");

    auto params = std::vector<Token>();
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (params.size() >= 225) {
                error(peek(), "Can't have more than 255 parameters");
            }

            params.push_back(
                consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    consume(TokenType::LEFT_BRACE,
            "Expect '{' before " + functionType + " body.");
    auto body = block();
    return std::make_shared<Stmt::Function>(name, std::move(params), body);
}

Stmt::StmtPtr Parser::statement() {
    if (match(TokenType::FOR)) {
        return forStmt();
    }

    if (match(TokenType::IF)) {
        return ifStmt();
    }

    if (match(TokenType::PRINT)) {
        return printStmt();
    }

    if (match(TokenType::RETURN)) {
        return returnStmt();
    }

    if (match(TokenType::WHILE)) {
        return whileStmt();
    }

    if (match(TokenType::LEFT_BRACE)) {
        return std::make_shared<Stmt::Block>(block());
    }

    return expressionStmt();
}

Stmt::StmtPtr Parser::expressionStmt() {
    Expr::ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<Stmt::Expr>(value);
}

Stmt::StmtPtr Parser::printStmt() {
    Expr::ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_shared<Stmt::Print>(value);
}

Stmt::StmtPtr Parser::ifStmt() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    auto thenBranch = statement();
    auto elseBranch = match(TokenType::ELSE) ? statement() : nullptr;

    return std::make_shared<Stmt::If>(condition, thenBranch, elseBranch);
}

Stmt::StmtPtr Parser::whileStmt() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    auto body = statement();

    return std::make_shared<Stmt::While>(condition, body);
}

Stmt::StmtPtr Parser::forStmt() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    Stmt::StmtPtr initializer;
    if (match(TokenType::SEMICOLON)) {
        initializer = nullptr;
    } else if (match(TokenType::VAR)) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStmt();
    }

    Expr::ExprPtr condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    Expr::ExprPtr increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    Stmt::StmtPtr body = statement();

    if (increment) {
        auto block = std::vector<Stmt::StmtPtr>();
        block.push_back(body);
        block.push_back(std::make_shared<Stmt::Expr>(increment));
        body = std::make_shared<Stmt::Block>(block);
    }

    if (!condition) {
        condition = std::make_shared<Expr::Literal>(true);
    }
    body = std::make_shared<Stmt::While>(condition, body);

    if (initializer) {
        auto block = std::vector<Stmt::StmtPtr>();
        block.push_back(initializer);
        block.push_back(body);
        body = std::make_shared<Stmt::Block>(block);
    }

    return body;
}

Stmt::StmtPtr Parser::returnStmt() {
    auto keyword = previous();

    Expr::ExprPtr value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_shared<Stmt::Return>(keyword, value);
}

std::vector<Stmt::StmtPtr> Parser::block() {
    auto statements = std::vector<Stmt::StmtPtr>();

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

Expr::ExprPtr Parser::assignment() {
    auto expr = logical_or();

    if (match(TokenType::EQUAL)) {
        Token equals = previous();
        auto value = assignment();

        if (auto lvalue =
                std::dynamic_pointer_cast<Expr::Variable const>(expr)) {
            Token name = lvalue->name;
            return std::make_shared<Expr::Assign>(name, value);
        }

        error(equals, "Invalid assignment target.");
    }

    return expr;
}

Expr::ExprPtr Parser::logical_or() {
    auto expr = logical_and();

    while (match(TokenType::OR)) {
        Token op = previous();
        auto right = logical_and();
        expr = std::make_shared<Expr::Logical>(expr, right, op);
    }

    return expr;
}
Expr::ExprPtr Parser::logical_and() {
    auto expr = equality();

    while (match(TokenType::AND)) {
        Token op = previous();
        auto right = equality();
        expr = std::make_shared<Expr::Logical>(expr, right, op);
    }

    return expr;
}

Expr::ExprPtr Parser::equality() {
    auto expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        auto op = previous();
        auto right = comparison();
        expr = std::make_shared<Expr::Binary>(expr, right, op);
    }

    return expr;
}

Expr::ExprPtr Parser::expression() { return assignment(); }

Expr::ExprPtr Parser::comparison() {
    auto expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                  TokenType::LESS_EQUAL})) {
        auto op = previous();
        auto right = term();
        expr = std::make_shared<Expr::Binary>(expr, right, op);
    }

    return expr;
}

Expr::ExprPtr Parser::term() {
    auto expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        auto op = previous();
        auto right = factor();
        expr = std::make_shared<Expr::Binary>(expr, right, op);
    }

    return expr;
}

Expr::ExprPtr Parser::factor() {
    auto expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        auto op = previous();
        auto right = factor();
        expr = std::make_shared<Expr::Binary>(expr, right, op);
    }

    return expr;
}

Expr::ExprPtr Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        auto op = previous();
        auto right = unary();
        return std::make_shared<Expr::Unary>(op, right);
    }

    return call();
}

Expr::ExprPtr Parser::call() {
    auto expr = primary();

    while (true) {
        if (match(TokenType::LEFT_PAREN)) {
            expr = finishCall(expr);
        } else if (match(TokenType::DOT)) {
            Token name = consume(TokenType::IDENTIFIER,
                                 "Expect property name after '.'.");
            expr = std::make_shared<Expr::Get>(expr, name);
        } else {
            break;
        }
    }

    return expr;
}

Expr::ExprPtr Parser::finishCall(Expr::ExprPtr callee) {
    auto arguments = std::vector<Expr::ExprPtr>();

    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }

    Token paren =
        consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_shared<Expr::Call>(callee, paren, std::move(arguments));
}

Expr::ExprPtr Parser::primary() {
    if (match(TokenType::FALSE))
        return std::make_shared<Expr::Literal>(false);
    if (match(TokenType::TRUE))
        return std::make_shared<Expr::Literal>(true);
    if (match(TokenType::NIL))
        return std::make_shared<Expr::Literal>(std::monostate{});

    if (match({TokenType::NUMBER, TokenType::STRING}))
        // tokens of these types are guaranteed to have a literal value
        return std::make_shared<Expr::Literal>(previous().literal.value());

    if (match(TokenType::IDENTIFIER)) {
        return std::make_shared<Expr::Variable>(previous());
    }

    if (match(TokenType::LEFT_PAREN)) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression");
        return std::make_shared<Expr::Grouping>(expr);
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
        default:
            advance();
        }
    }
}

auto Parser::error(Token token, const char *message) -> Parser::ParserError {
    handler.error(token, message);
    return ParserError();
}

Token Parser::consume(TokenType type, std::string message) {
    if (check(type))
        return advance();

    throw error(peek(), message.c_str());
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

Token Parser::peek() { return tokens.at(current); }

Token Parser::previous() { return tokens.at(current - 1); }
