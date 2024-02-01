#ifndef CLOXPP_INTERPRETER_H
#define CLOXPP_INTERPRETER_H

#include <vector>

#include "environment.hpp"
#include "error_handler.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "value.hpp"

class Interpreter : Expr::Visitor<Value>, Stmt::Visitor<void> {
public:
    Interpreter(ErrorHandler &handler);

    void interpret(const std::vector<Stmt::StmtPtr> &statements);

private:
    ErrorHandler &handler;
    Environment environment;

    Value evaluate(const Expr::BaseExpr &expr);
    Value visit(const Expr::Binary &expr);
    Value visit(const Expr::Grouping &expr);
    Value visit(const Expr::Unary &expr);
    Value visit(const Expr::Literal &expr);
    Value visit(const Expr::Variable &expr);
    Value visit(const Expr::Assign &expr);

    void execute(const Stmt::BaseStmt &stmt);
    void visit(const Stmt::Expr &stmt);
    void visit(const Stmt::Print &stmt);
    void visit(const Stmt::Var &stmt);
    void visit(const Stmt::Block &stmt);

    void executeBlock(const std::vector<Stmt::StmtPtr> &statements,
                      Environment *environment);
};

#endif // !CLOXPP_INTERPRETER_H
