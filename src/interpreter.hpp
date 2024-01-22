#ifndef CLOXPP_INTERPRETER_H
#define CLOXPP_INTERPRETER_H

#include "error_handler.hpp"
#include "expr.hpp"
#include "literal.hpp"

using LoxValue = Literal;

class Interpreter : Expr::Visitor<LoxValue> {
public:
    Interpreter() = default;
    Interpreter(ErrorHandler *handler) : handler_(handler){};

    void interpret(Expr::ExprPtr expr);

    LoxValue evaluate(const Expr::BaseExpr &expr);

    LoxValue visitBinary(const Expr::Binary &expr);
    LoxValue visitGrouping(const Expr::Grouping &expr);
    LoxValue visitUnary(const Expr::Unary &expr);
    LoxValue visitLiteral(const Expr::Literal &expr);

private:
    ErrorHandler *handler_;
};

#endif // !CLOXPP_INTERPRETER_H
