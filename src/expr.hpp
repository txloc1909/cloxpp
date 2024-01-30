#ifndef CLOXPP_EXPR_H
#define CLOXPP_EXPR_H

#include <memory>

#include "token.hpp"
#include "value.hpp"

#define DEFINE_NODE_ACCEPT_METHOD(ReturnType)                                  \
    ReturnType accept(const Visitor<ReturnType> &visitor) const override {     \
        return visitor.visit(*this);                                           \
    }

using LiteralValue = Literal; // Alias

namespace Expr {

struct Binary;
struct Unary;
struct Grouping;
struct Literal;

template <typename R>
class Visitor {
public:
    virtual R visit(const Binary &expr) const = 0;
    virtual R visit(const Grouping &expr) const = 0;
    virtual R visit(const Unary &expr) const = 0;
    virtual R visit(const Literal &expr) const = 0;

    virtual ~Visitor() = default;
};

struct BaseExpr {
    virtual ~BaseExpr() = default;
    virtual Value accept(const Visitor<Value> &visitor) const = 0;
};
using ExprPtr = std::unique_ptr<BaseExpr>;

struct Binary : BaseExpr {
    const ExprPtr left;
    const ExprPtr right;
    const Token op;

    Binary(ExprPtr left, ExprPtr right, Token op)
        : left(std::move(left)), right(std::move(right)), op(op) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
};

struct Grouping : BaseExpr {
    const ExprPtr inner;

    Grouping(ExprPtr expr) : inner(std::move(expr)) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
};

struct Unary : BaseExpr {
    const Token op;
    const ExprPtr right;

    Unary(Token op, ExprPtr right) : op(op), right(std::move(right)) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
};

struct Literal : BaseExpr {
    const LiteralValue value;

    Literal(LiteralValue value) : value(value) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
};

} // namespace Expr

#undef DEFINE_NODE_ACCEPT_METHOD
#endif // !CLOXPP_EXPR_H
