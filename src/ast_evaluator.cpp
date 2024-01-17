#include "expr.hpp"
#include <iostream>
#include <stdexcept>

using LoxValue = Literal;

class ASTEvaluator : Expr::Visitor<LoxValue> {
public:
    LoxValue visit(const Expr::BaseExpr &expr) { return expr.accept(*this); }

    LoxValue visitBinary(const Expr::Binary &expr) override {
        double left = std::get<double>(this->visit(*(expr.left_)).value());
        double right = std::get<double>(this->visit(*(expr.right_)).value());

        std::cout << left << " " << expr.op_.lexeme << " " << right << "\n";
        if (expr.op_.type == TokenType::PLUS) {
            return left + right;
        } else if (expr.op_.type == TokenType::MINUS) {
            return left - right;
        } else if (expr.op_.type == TokenType::STAR) {
            return left * right;
        }

        throw std::runtime_error("Unsupported op");
    }

    LoxValue visitGrouping(const Expr::Grouping &expr) override {
        LoxValue val = expr.accept(*this);
        std::cout << "(" << val << ")\n";
        return val;
    }

    LoxValue visitUnary(const Expr::Unary &expr) override {
        double right = std::get<double>(expr.right_->accept(*this).value());

        std::cout << expr.op_.lexeme << " " << right << "\n";
        if (expr.op_.type == TokenType::MINUS) {
            return 0.0 - right;
        }

        throw std::runtime_error("Unsupported op");
    }

    LoxValue visitLiteral(const Expr::Literal &expr) override {
        std::cout << expr.value_ << "\n";
        return expr.value_;
    }
};

int main() {
    const auto expr = std::make_unique<Expr::Binary>(
        std::make_unique<Expr::Unary>(
            Token(TokenType::MINUS, "-", std::nullopt, 1),
            std::make_unique<Expr::Literal>(123.0f)),
        std::make_unique<Expr::Grouping>(
            std::make_unique<Expr::Literal>(45.67f)),
        Token(TokenType::PLUS, "+", std::nullopt, 1));

    auto evaluator = std::make_unique<ASTEvaluator>();

    std::cout << evaluator->visit(*expr);
    return 0;
}
