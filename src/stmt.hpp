#ifndef CLOXPP_STMT_H
#define CLOXPP_STMT_H

#include <memory>
#include <vector>

#include "expr.hpp"

#define DEFINE_NODE_ACCEPT_METHOD(ReturnType)                                  \
    ReturnType accept(Visitor<ReturnType> &visitor) const override {           \
        return visitor.visit(*this);                                           \
    }

namespace Stmt {

using Expr::ExprPtr;

struct Expr;
struct Print;
struct Var;
struct Block;
struct If;
struct While;

template <typename R>
class Visitor {
public:
    virtual R visit(const Expr &expr) = 0;
    virtual R visit(const Print &expr) = 0;
    virtual R visit(const Var &expr) = 0;
    virtual R visit(const Block &expr) = 0;
    virtual R visit(const If &expr) = 0;
    virtual R visit(const While &expr) = 0;

    virtual ~Visitor() = default;
};

struct BaseStmt {
    virtual ~BaseStmt() = default;
    virtual void accept(Visitor<void> &visitor) const = 0;
};
using StmtPtr = std::unique_ptr<BaseStmt>;

struct Expr : BaseStmt {
    const ExprPtr expr;

    Expr(ExprPtr expr) : expr(std::move(expr)) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Print : BaseStmt {
    const ExprPtr expr;

    Print(ExprPtr expr) : expr(std::move(expr)) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Var : BaseStmt {
    const Token name;
    const ExprPtr initializer;

    Var(Token name, ExprPtr initializer)
        : name(name), initializer(std::move(initializer)) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Block : BaseStmt {
    const std::vector<StmtPtr> statements;

    Block(std::vector<StmtPtr> statements)
        : statements(std::move(statements)) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct If : BaseStmt {
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;

    If(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct While : BaseStmt {
    ExprPtr condition;
    StmtPtr body;

    While(ExprPtr condition, StmtPtr body)
        : condition(std::move(condition)), body(std::move(body)) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

} // namespace Stmt

#undef DEFINE_NODE_ACCEPT_METHOD
#endif // !CLOXPP_STMT_H
