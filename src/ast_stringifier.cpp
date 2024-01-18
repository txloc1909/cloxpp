#include "ast_stringifier.hpp"
#include "expr.hpp"

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
