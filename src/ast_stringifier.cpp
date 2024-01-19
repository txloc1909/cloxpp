#include "ast_stringifier.hpp"
#include "expr.hpp"

std::string ASTStringifier::stringify(const Expr::BaseExpr &expr) {
    return expr.accept(*this);
}

std::string ASTStringifier::visitBinary(const Expr::Binary &expr) {
    return "(" + expr.op_.lexeme + " " + this->stringify(*expr.left_) + " " +
           this->stringify(*expr.right_) + ")";
}

std::string ASTStringifier::visitGrouping(const Expr::Grouping &expr) {
    return "(" + this->stringify(*expr.inner_) + ")";
}

std::string ASTStringifier::visitUnary(const Expr::Unary &expr) {
    return "(" + expr.op_.lexeme + " " + this->stringify(*expr.right_) + ")";
}

std::string ASTStringifier::visitLiteral(const Expr::Literal &expr) {
    return stringifyLiteral(expr.value_);
}
