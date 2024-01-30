#ifndef CLOXPP_STMT_H
#define CLOXPP_STMT_H

#include <memory>

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

template <typename R>
class Visitor {
public:
    virtual R visit(const Expr &expr) = 0;
    virtual R visit(const Print &expr) = 0;
    virtual R visit(const Var &expr) = 0;

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

} // namespace Stmt

#undef DEFINE_NODE_ACCEPT_METHOD
#endif // !CLOXPP_STMT_H
