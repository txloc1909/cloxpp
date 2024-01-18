#include "ast_stringifier.hpp"
#include "expr.hpp"
#include <iostream>

std::string ASTStringifier::visit(const Expr::BaseExpr &expr) {
    return expr.accept(*this);
}

std::string ASTStringifier::visitBinary(const Expr::Binary &expr) {
    return "(" + expr.op_.lexeme + " " + this->visit(*expr.left_) + " " +
           this->visit(*expr.right_) + ")";
}

std::string ASTStringifier::visitGrouping(const Expr::Grouping &expr) {
    return "(" + this->visit(*expr.inner_) + ")";
}

std::string ASTStringifier::visitUnary(const Expr::Unary &expr) {
    return "(" + expr.op_.lexeme + " " + this->visit(*expr.right_) + ")";
}

std::string ASTStringifier::visitLiteral(const Expr::Literal &expr) {
    return stringify(expr.value_);
}

int main() {
    const auto expr = std::make_unique<Expr::Binary>(
        std::make_unique<Expr::Unary>(
            Token(TokenType::MINUS, "-", std::nullopt, 1),
            std::make_unique<Expr::Literal>(123.0)),
        std::make_unique<Expr::Grouping>(
            std::make_unique<Expr::Literal>(45.67)),
        Token(TokenType::STAR, "*", std::nullopt, 1));

    ASTStringifier printer{};
    std::cout << printer.visit(*expr);
    return 0;
}
