#ifndef CLOXPP_INTERPRETER_H
#define CLOXPP_INTERPRETER_H

#include <vector>

#include "error_handler.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "value.hpp"

class Interpreter : Expr::Visitor<Value>, Stmt::Visitor<void> {
public:
    Interpreter(ErrorHandler &handler) : handler_(handler){};

    void interpret(const std::vector<Stmt::StmtPtr> &statements) const;

private:
    ErrorHandler &handler_;

    Value evaluate(const Expr::BaseExpr &expr) const;
    Value visit(const Expr::Binary &expr) const;
    Value visit(const Expr::Grouping &expr) const;
    Value visit(const Expr::Unary &expr) const;
    Value visit(const Expr::Literal &expr) const;

    void execute(const Stmt::BaseStmt &stmt) const;
    void visit(const Stmt::Expr &stmt) const;
    void visit(const Stmt::Print &stmt) const;
};

#endif // !CLOXPP_INTERPRETER_H
