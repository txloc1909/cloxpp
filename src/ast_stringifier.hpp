#ifndef CLOXPP_AST_STRINGIFIER_H
#define CLOXPP_AST_STRINGIFIER_H

#include <string>

#include "expr.hpp"

class ASTStringifier : Expr::Visitor<std::string> {
public:
    std::string stringify(const Expr::BaseExpr &expr) const;

    std::string visitBinary(const Expr::Binary &expr) const override;
    std::string visitGrouping(const Expr::Grouping &expr) const override;
    std::string visitUnary(const Expr::Unary &expr) const override;
    std::string visitLiteral(const Expr::Literal &expr) const override;
};

#endif // !CLOXPP_AST_STRINGIFIER_H
