#ifndef CLOXPP_AST_STRINGIFIER_H
#define CLOXPP_AST_STRINGIFIER_H

#include "expr.hpp"
#include <string>

class ASTStringifier : Expr::Visitor<std::string> {
public:
    std::string stringify(const Expr::BaseExpr &expr);

    std::string visitBinary(const Expr::Binary &expr) override;
    std::string visitGrouping(const Expr::Grouping &expr) override;
    std::string visitUnary(const Expr::Unary &expr) override;
    std::string visitLiteral(const Expr::Literal &expr) override;
};

#endif // !CLOXPP_AST_STRINGIFIER_H
