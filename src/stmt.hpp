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
struct Function;
struct Return;

template <typename R>
class Visitor {
public:
    virtual R visit(const Expr &expr) = 0;
    virtual R visit(const Print &expr) = 0;
    virtual R visit(const Var &expr) = 0;
    virtual R visit(const Block &expr) = 0;
    virtual R visit(const If &expr) = 0;
    virtual R visit(const While &expr) = 0;
    virtual R visit(const Function &expr) = 0;
    virtual R visit(const Return &expr) = 0;

    virtual ~Visitor() = default;
};

struct BaseStmt {
    virtual ~BaseStmt() = default;
    virtual void accept(Visitor<void> &visitor) const = 0;
};
using StmtPtr = std::shared_ptr<BaseStmt>;

struct Expr : BaseStmt {
    const ExprPtr expr;

    Expr(ExprPtr expr) : expr(expr) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Print : BaseStmt {
    const ExprPtr expr;

    Print(ExprPtr expr) : expr(expr) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Var : BaseStmt {
    const Token name;
    const ExprPtr initializer;

    Var(Token name, ExprPtr initializer)
        : name(name), initializer(initializer) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Block : BaseStmt {
    const std::vector<StmtPtr> statements;

    Block(std::vector<StmtPtr> statements)
        : statements(std::move(statements)) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct If : BaseStmt {
    const ExprPtr condition;
    const StmtPtr thenBranch;
    const StmtPtr elseBranch;

    If(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
    }
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct While : BaseStmt {
    const ExprPtr condition;
    const StmtPtr body;

    While(ExprPtr condition, StmtPtr body) : condition(condition), body(body) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Function : BaseStmt {
    const Token name;
    const std::vector<Token> params;
    const std::vector<StmtPtr> body;

    Function(Token name, std::vector<Token> params, std::vector<StmtPtr> body)
        : name(name), params(std::move(params)), body(std::move(body)){};
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Return : BaseStmt {
    const Token keyword;
    const ExprPtr value;

    Return(Token keyword, ExprPtr value) : keyword(keyword), value(value) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

} // namespace Stmt

#undef DEFINE_NODE_ACCEPT_METHOD
#endif // !CLOXPP_STMT_H
