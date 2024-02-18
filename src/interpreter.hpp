#ifndef CLOXPP_INTERPRETER_H
#define CLOXPP_INTERPRETER_H

#include <unordered_map>
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
    EnvironmentPtr new_env;
    EnvironmentPtr saved_env;

public:
    ScopeManager(Interpreter &interpreter, EnvironmentPtr new_env);
    ~ScopeManager();
};

class Interpreter : Expr::Visitor<Value>, Stmt::Visitor<void> {
public:
    Interpreter(ErrorHandler &handler);
    ~Interpreter();

    EnvironmentPtr globalEnvironment() const;
    void interpret(const std::vector<Stmt::StmtPtr> &statements);
    void executeBlock(const std::vector<Stmt::StmtPtr> &statements,
                      EnvironmentPtr environment);
    void resolve(Expr::ExprPtr expr, int depth);

private:
    ErrorHandler &handler;
    EnvironmentPtr globals_;
    EnvironmentPtr currentEnvironment;
    std::unordered_map<Expr::ExprPtr, int> locals;

    Value evaluate(const Expr::ExprPtr expr);
    Value visit(Expr::BinaryPtr expr) override;
    Value visit(Expr::GroupingPtr expr) override;
    Value visit(Expr::UnaryPtr expr) override;
    Value visit(Expr::LiteralPtr expr) override;
    Value visit(Expr::VariablePtr expr) override;
    Value visit(Expr::AssignPtr expr) override;
    Value visit(Expr::LogicalPtr expr) override;
    Value visit(Expr::CallPtr expr) override;
    Value visit(Expr::GetPtr expr) override;
    Value visit(Expr::SetPtr expr) override;
    Value visit(Expr::ThisPtr expr) override;

    void execute(const Stmt::StmtPtr stmt);
    void visit(Stmt::ExprPtr stmt) override;
    void visit(Stmt::PrintPtr stmt) override;
    void visit(Stmt::VarPtr stmt) override;
    void visit(Stmt::BlockPtr stmt) override;
    void visit(Stmt::IfPtr stmt) override;
    void visit(Stmt::WhilePtr stmt) override;
    void visit(Stmt::FunctionPtr stmt) override;
    void visit(Stmt::ReturnPtr stmt) override;
    void visit(Stmt::ClassPtr stmt) override;

    Value lookUpVariable(const Token &name, const Expr::ExprPtr &expr);

    friend ScopeManager::ScopeManager(Interpreter &interpreter,
                                      EnvironmentPtr new_env);
    friend ScopeManager::~ScopeManager();
};

#endif // !CLOXPP_INTERPRETER_H
