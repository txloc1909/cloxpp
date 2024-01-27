#ifndef CLOXPP_STMT_H
#define CLOXPP_STMT_H

#include <memory>

#include "expr.hpp"

namespace Stmt {

using Expr::ExprPtr;

struct Expr;
struct Print;

template <typename R>
class Visitor {
public:
    virtual R visitExpr(const Expr &expr) const = 0;
    virtual R visitPrint(const Print &expr) const = 0;

    virtual ~Visitor() = default;
};

struct BaseStmt {
    virtual ~BaseStmt() = default;
    virtual void accept(const Visitor<void> &visitor) const = 0;
};

using StmtPtr = std::unique_ptr<BaseStmt>;

struct Expr : BaseStmt {
    const ExprPtr expr_;

    Expr(ExprPtr expr) : expr_(std::move(expr)) {}
    void accept(const Visitor<void> &visitor) const override {
        return visitor.visitExpr(*this);
    };
};

struct Print : BaseStmt {
    const ExprPtr expr_;

    Print(ExprPtr expr) : expr_(std::move(expr)) {}
    void accept(const Visitor<void> &visitor) const override {
        return visitor.visitPrint(*this);
    }
};

} // namespace Stmt

#endif // !CLOXPP_STMT_H
