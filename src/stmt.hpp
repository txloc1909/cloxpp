#ifndef CLOXPP_STMT_H
#define CLOXPP_STMT_H

#include <memory>

#include "expr.hpp"

namespace Stmt {

using Expr::ExprPtr;

class Expr;
class Print;

template <typename R> class Visitor {
public:
    virtual R visitExpr(const Expr &expr) const = 0;
    virtual R visitPrint(const Print &expr) const = 0;

    virtual ~Visitor() = default;
};

class BaseStmt {
public:
    virtual ~BaseStmt() = default;
    virtual void accept(const Visitor<void> &visitor) const = 0;
};

using StmtPtr = std::unique_ptr<BaseStmt>;

class Expr : public BaseStmt {
public:
    Expr(ExprPtr expr) : expr_(std::move(expr)) {}
    void accept(const Visitor<void> &visitor) const override {
        return visitor.visitExpr(*this);
    };

    ExprPtr expr_;
};

class Print : public BaseStmt {
public:
    Print(ExprPtr expr) : expr_(std::move(expr)) {}
    void accept(const Visitor<void> &visitor) const override {
        return visitor.visitPrint(*this);
    }

    ExprPtr expr_;
};

} // namespace Stmt

#endif // !CLOXPP_STMT_H
