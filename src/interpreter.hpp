#ifndef CLOXPP_INTERPRETER_H
#define CLOXPP_INTERPRETER_H

#include "error_handler.hpp"
#include "expr.hpp"
#include "literal.hpp"

using LoxValue = Literal;

class Interpreter : Expr::Visitor<LoxValue> {
public:
    Interpreter(ErrorHandler &handler) : handler_(handler){};

    void interpret(Expr::ExprPtr expr) const;

private:
    ErrorHandler &handler_;

    LoxValue evaluate(const Expr::BaseExpr &expr) const;
    LoxValue visitBinary(const Expr::Binary &expr) const;
    LoxValue visitGrouping(const Expr::Grouping &expr) const;
    LoxValue visitUnary(const Expr::Unary &expr) const;
    LoxValue visitLiteral(const Expr::Literal &expr) const;
};

#endif // !CLOXPP_INTERPRETER_H
