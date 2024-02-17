#ifndef CLOXPP_RESOLVER_H
#define CLOXPP_RESOLVER_H

#include <vector>

#include "expr.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"

enum FunctionType {
    NONE,
    FUNCTION,
};

class Resolver : Expr::Visitor<void>, Stmt::Visitor<void> {
public:
    Resolver(Interpreter &interpreter, ErrorHandler &handler)
        : handler(handler), interpreter(interpreter),
          currentFunction(FunctionType::NONE){};
    void resolve(const std::vector<Stmt::StmtPtr> &statements);

private:
    ErrorHandler &handler;
    Interpreter &interpreter;
    std::vector<std::unordered_map<std::string, bool>> scopes{};
    FunctionType currentFunction;

    void resolve(const Expr::ExprPtr expr);
    void visit(Expr::BinaryPtr expr) override;
    void visit(Expr::GroupingPtr expr) override;
    void visit(Expr::UnaryPtr expr) override;
    void visit(Expr::LiteralPtr expr) override;
    void visit(Expr::VariablePtr expr) override;
    void visit(Expr::AssignPtr expr) override;
    void visit(Expr::LogicalPtr expr) override;
    void visit(Expr::CallPtr expr) override;

    void resolve(const Stmt::StmtPtr expr);
    void visit(Stmt::ExprPtr stmt) override;
    void visit(Stmt::PrintPtr stmt) override;
    void visit(Stmt::VarPtr stmt) override;
    void visit(Stmt::BlockPtr stmt) override;
    void visit(Stmt::IfPtr stmt) override;
    void visit(Stmt::WhilePtr stmt) override;
    void visit(Stmt::FunctionPtr stmt) override;
    void visit(Stmt::ReturnPtr stmt) override;

    void resolveLocal(Expr::ExprPtr expr, const Token &name);
    void resolveFunction(Stmt::FunctionPtr function, FunctionType type);
    void declare(const Token &name);
    void define(const Token &name);
    void beginScope();
    void endScope();
};

#endif // !CLOXPP_RESOLVER_H
