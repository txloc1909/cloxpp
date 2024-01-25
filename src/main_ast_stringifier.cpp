#include <iostream>

#include "ast_stringifier.hpp"
#include "expr.hpp"

int main() {
    const auto expr = std::make_unique<Expr::Binary>(
        std::make_unique<Expr::Unary>(Token(TokenType::MINUS, "-", {}, 1),
                                      std::make_unique<Expr::Literal>(123.0)),
        std::make_unique<Expr::Grouping>(
            std::make_unique<Expr::Literal>(45.67)),
        Token(TokenType::STAR, "*", {}, 1));

    std::cout << ASTStringifier().stringify(*expr);
    return 0;
}