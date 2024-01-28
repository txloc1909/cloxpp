#ifndef CLOXPP_INTERPRETER_H
#define CLOXPP_INTERPRETER_H

#include <vector>

#include "error_handler.hpp"
#include "expr.hpp"
#include "literal.hpp"
#include "stmt.hpp"

using LoxValue = Literal;

class Interpreter : Expr::Visitor<LoxValue>, Stmt::Visitor<void> {
public:
    Interpreter(ErrorHandler &handler) : handler_(handler){};

    void interpret(const std::vector<Stmt::StmtPtr> &statements) const;

private:
    ErrorHandler &handler_;

    LoxValue evaluate(const Expr::BaseExpr &expr) const;
    LoxValue visit(const Expr::Binary &expr) const;
    LoxValue visit(const Expr::Grouping &expr) const;
    LoxValue visit(const Expr::Unary &expr) const;
    LoxValue visit(const Expr::Literal &expr) const;

    void execute(const Stmt::BaseStmt &stmt) const;
    void visit(const Stmt::Expr &stmt) const;
    void visit(const Stmt::Print &stmt) const;
};

#endif // !CLOXPP_INTERPRETER_H
