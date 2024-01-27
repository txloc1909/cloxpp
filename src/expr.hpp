#ifndef CLOXPP_EXPR_H
#define CLOXPP_EXPR_H

#include <memory>

#include "literal.hpp"
#include "token.hpp"

using Value = Literal; // A hack to avoid `Literal` in `literal.hpp`

namespace Expr {

struct Binary;
struct Unary;
struct Grouping;
struct Literal;

template <typename R>
class Visitor {
public:
    virtual R visitBinary(const Binary &expr) const = 0;
    virtual R visitGrouping(const Grouping &expr) const = 0;
    virtual R visitUnary(const Unary &expr) const = 0;
    virtual R visitLiteral(const Literal &expr) const = 0;

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
    Value accept(const Visitor<Value> &visitor) const override {
        return visitor.visitBinary(*this);
    };
};

struct Grouping : BaseExpr {
    const ExprPtr inner_;

    Grouping(ExprPtr expr) : inner_(std::move(expr)) {}
    Value accept(const Visitor<Value> &visitor) const override {
        return visitor.visitGrouping(*this);
    };
};

struct Unary : BaseExpr {
    const Token op_;
    const ExprPtr right_;

    Unary(Token op, ExprPtr right) : op_(op), right_(std::move(right)) {}
    Value accept(const Visitor<Value> &visitor) const override {
        return visitor.visitUnary(*this);
    };
};

struct Literal : BaseExpr {
    const Value value_;

    Literal(Value value) : value_(value) {}
    Value accept(const Visitor<Value> &visitor) const override {
        return visitor.visitLiteral(*this);
    };
};

} // namespace Expr

#endif // !CLOXPP_EXPR_H
