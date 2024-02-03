#ifndef CLOXPP_INTERPRETER_H
#define CLOXPP_INTERPRETER_H

#include <vector>

#include "environment.hpp"
#include "error_handler.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "value.hpp"

class Interpreter;

class ScopeManager {
private:
    Interpreter &interpreter;
    Environment *new_env;
    Environment *saved_env;

public:
    ScopeManager(Interpreter &interpreter, Environment *new_env);
    ~ScopeManager();
};

class Interpreter : Expr::Visitor<Value>, Stmt::Visitor<void> {
public:
    Interpreter(ErrorHandler &handler);
    ~Interpreter();

    void interpret(const std::vector<Stmt::StmtPtr> &statements);

private:
    ErrorHandler &handler;
    Environment *environment;

    Value evaluate(const Expr::BaseExpr &expr);
    Value visit(const Expr::Binary &expr) override;
    Value visit(const Expr::Grouping &expr) override;
    Value visit(const Expr::Unary &expr) override;
    Value visit(const Expr::Literal &expr) override;
    Value visit(const Expr::Variable &expr) override;
    Value visit(const Expr::Assign &expr) override;
    Value visit(const Expr::Logical &expr) override;
    Value visit(const Expr::Call &expr) override;

    void execute(const Stmt::BaseStmt &stmt);
    void visit(const Stmt::Expr &stmt) override;
    void visit(const Stmt::Print &stmt) override;
    void visit(const Stmt::Var &stmt) override;
    void visit(const Stmt::Block &stmt) override;
    void visit(const Stmt::If &stmt) override;
    void visit(const Stmt::While &stmt) override;

    void executeBlock(const std::vector<Stmt::StmtPtr> &statements,
                      Environment *environment);

    friend ScopeManager::ScopeManager(Interpreter &interpreter,
                                      Environment *new_env);
    friend ScopeManager::~ScopeManager();
};

#endif // !CLOXPP_INTERPRETER_H
