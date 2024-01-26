#include <iostream>
#include <stdexcept>

#include "expr.hpp"

using LoxValue = Literal;

class ASTEvaluator : Expr::Visitor<LoxValue> {
public:
    LoxValue evaluate(const Expr::BaseExpr &expr) const {
        return expr.accept(*this);
    }

    LoxValue visitBinary(const Expr::Binary &expr) const override {
        auto left = std::get<double>(this->evaluate(*expr.left_));
        auto right = std::get<double>(this->evaluate(*expr.right_));

        if (expr.op_.type == TokenType::PLUS) {
            return left + right;
        } else if (expr.op_.type == TokenType::MINUS) {
            return left - right;
        } else if (expr.op_.type == TokenType::STAR) {
            return left * right;
        }

        throw std::runtime_error("Unsupported op");
    }

    LoxValue visitGrouping(const Expr::Grouping &expr) const override {
        return this->evaluate(*expr.inner_);
    }

    LoxValue visitUnary(const Expr::Unary &expr) const override {
        auto right = std::get<double>(this->evaluate(*expr.right_));
        if (expr.op_.type == TokenType::MINUS) {
            return -right;
        }

        throw std::runtime_error("Unsupported op");
    }

    LoxValue visitLiteral(const Expr::Literal &expr) const override {
        return expr.value_;
    }

    ~ASTEvaluator() = default;
};

int main() {
    const auto expr = std::make_unique<Expr::Binary>(
        std::make_unique<Expr::Unary>(Token(TokenType::MINUS, "-", {}, 1),
                                      std::make_unique<Expr::Literal>(123.0)),
        std::make_unique<Expr::Grouping>(
            std::make_unique<Expr::Literal>(45.67)),
        Token(TokenType::STAR, "*", {}, 1));

    std::cout << ASTEvaluator().evaluate(*expr);
    return 0;
}
