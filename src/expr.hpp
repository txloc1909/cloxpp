#ifndef CLOXPP_EXPR_H
#define CLOXPP_EXPR_H

#include <memory>

#include "token.hpp"
#include "value.hpp"

#define DEFINE_NODE_ACCEPT_METHOD(ReturnType)                                  \
    ReturnType accept(const Visitor<ReturnType> &visitor) const override {     \
        return visitor.visit(*this);                                           \
    }

using Value = Literal; // A hack to avoid `Literal` in `literal.hpp`

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
    const ExprPtr left_;
    const ExprPtr right_;
    const Token op_;

    Binary(ExprPtr left, ExprPtr right, Token op)
        : left_(std::move(left)), right_(std::move(right)), op_(op) {}
    DEFINE_NODE_ACCEPT_METHOD(Value);
};

struct Grouping : BaseExpr {
    const ExprPtr inner_;

    Grouping(ExprPtr expr) : inner_(std::move(expr)) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
};

struct Unary : BaseExpr {
    const Token op_;
    const ExprPtr right_;

    Unary(Token op, ExprPtr right) : op_(op), right_(std::move(right)) {}
    DEFINE_NODE_ACCEPT_METHOD(Value);
};

struct Literal : BaseExpr {
    const Value value_;

    Literal(Value value) : value_(value) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
};

} // namespace Expr

#undef DEFINE_NODE_ACCEPT_METHOD
#endif // !CLOXPP_EXPR_H
