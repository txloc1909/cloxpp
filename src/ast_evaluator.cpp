#include "expr.hpp"
#include <iostream>
#include <stdexcept>

using LoxValue = Literal;

template <typename T> static T getFromValue(const LoxValue &value) {
    if (value.has_value() && std::holds_alternative<T>(value.value())) {
        return std::get<T>(value.value());
    }

    throw std::runtime_error("Unexpected value");
}

class ASTEvaluator : Expr::Visitor<LoxValue> {
public:
    LoxValue visit(const Expr::BaseExpr &expr) { return expr.accept(*this); }

    LoxValue visitBinary(const Expr::Binary &expr) override {
        auto left = getFromValue<double>(this->visit(*expr.left_));
        auto right = getFromValue<double>(this->visit(*expr.right_));

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
        return this->visit(*expr.inner_);
    }

    LoxValue visitUnary(const Expr::Unary &expr) override {
        auto right = getFromValue<double>(this->visit(*expr.right_));
        if (expr.op_.type == TokenType::MINUS) {
            return -right;
        }

        throw std::runtime_error("Unsupported op");
    }

    LoxValue visitLiteral(const Expr::Literal &expr) override {
        return expr.value_;
    }

    ~ASTEvaluator() = default;
};

int main() {
    const auto expr = std::make_unique<Expr::Binary>(
        std::make_unique<Expr::Unary>(
            Token(TokenType::MINUS, "-", std::nullopt, 1),
            std::make_unique<Expr::Literal>(123.0)),
        std::make_unique<Expr::Grouping>(
            std::make_unique<Expr::Literal>(45.67)),
        Token(TokenType::STAR, "*", std::nullopt, 1));

    ASTEvaluator evaluator{};
    std::cout << evaluator.visit(*expr);
    return 0;
}
