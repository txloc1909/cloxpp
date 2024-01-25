#ifndef CLOXPP_EXPR_H
#define CLOXPP_EXPR_H

#include <memory>

#include "literal.hpp"
#include "token.hpp"

using Value = Literal; // A hack to avoid `Literal` in `literal.hpp`

namespace Expr {

class Binary;
class Unary;
class Grouping;
class Literal;

template <typename R> class Visitor {
public:
    virtual R visitBinary(const Binary &expr) = 0;
    virtual R visitGrouping(const Grouping &expr) = 0;
    virtual R visitUnary(const Unary &expr) = 0;
    virtual R visitLiteral(const Literal &expr) = 0;

    virtual ~Visitor() = default;
};

class BaseExpr {
public:
    virtual ~BaseExpr() = default;
    virtual Value accept(Visitor<Value> &visitor) const = 0;
    virtual std::string accept(Visitor<std::string> &visitor) const = 0;
};

using ExprPtr = std::unique_ptr<BaseExpr>;

class Binary : public BaseExpr {
public:
    Binary(ExprPtr left, ExprPtr right, Token op)
        : left_(std::move(left)), right_(std::move(right)), op_(op) {}
    Value accept(Visitor<Value> &visitor) const override {
        return visitor.visitBinary(*this);
    };
    std::string accept(Visitor<std::string> &visitor) const override {
        return visitor.visitBinary(*this);
    };

    ExprPtr left_;
    ExprPtr right_;
    Token op_;
};

class Grouping : public BaseExpr {
public:
    Grouping(ExprPtr expr) : inner_(std::move(expr)) {}
    Value accept(Visitor<Value> &visitor) const override {
        return visitor.visitGrouping(*this);
    };
    std::string accept(Visitor<std::string> &visitor) const override {
        return visitor.visitGrouping(*this);
    };

    ExprPtr inner_;
};

class Unary : public BaseExpr {
public:
    Unary(Token op, ExprPtr right) : op_(op), right_(std::move(right)) {}
    Value accept(Visitor<Value> &visitor) const override {
        return visitor.visitUnary(*this);
    };
    std::string accept(Visitor<std::string> &visitor) const override {
        return visitor.visitUnary(*this);
    };

    Token op_;
    ExprPtr right_;
};

class Literal : public BaseExpr {
public:
    Literal(Value value) : value_(value) {}
    Value accept(Visitor<Value> &visitor) const override {
        return visitor.visitLiteral(*this);
    };
    std::string accept(Visitor<std::string> &visitor) const override {
        return visitor.visitLiteral(*this);
    };

    Value value_;
};

} // namespace Expr

#endif // !CLOXPP_EXPR_H
