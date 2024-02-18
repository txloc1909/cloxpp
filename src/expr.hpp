#ifndef CLOXPP_EXPR_H
#define CLOXPP_EXPR_H

#include <memory>
#include <vector>

#include "token.hpp"
#include "value.hpp"

#define DEFINE_NODE_ACCEPT_METHOD(ReturnType)                                  \
    ReturnType accept(Visitor<ReturnType> &visitor) const override {           \
        return visitor.visit(shared_from_this());                              \
    }

using LiteralValue = Literal; // Alias

namespace Expr {

struct Binary;
struct Unary;
struct Grouping;
struct Literal;
struct Variable;
struct Assign;
struct Logical;
struct Call;
struct Get;

using BinaryPtr = std::shared_ptr<Binary const>;
using UnaryPtr = std::shared_ptr<Unary const>;
using GroupingPtr = std::shared_ptr<Grouping const>;
using LiteralPtr = std::shared_ptr<Literal const>;
using VariablePtr = std::shared_ptr<Variable const>;
using AssignPtr = std::shared_ptr<Assign const>;
using LogicalPtr = std::shared_ptr<Logical const>;
using CallPtr = std::shared_ptr<Call const>;
using GetPtr = std::shared_ptr<Get const>;

template <typename R>
class Visitor {
public:
    virtual R visit(BinaryPtr expr) = 0;
    virtual R visit(GroupingPtr expr) = 0;
    virtual R visit(UnaryPtr expr) = 0;
    virtual R visit(LiteralPtr expr) = 0;
    virtual R visit(VariablePtr expr) = 0;
    virtual R visit(AssignPtr expr) = 0;
    virtual R visit(LogicalPtr expr) = 0;
    virtual R visit(CallPtr expr) = 0;
    virtual R visit(GetPtr expr) = 0;

    virtual ~Visitor() = default;
};

struct BaseExpr {
    virtual ~BaseExpr() = default;
    virtual Value accept(Visitor<Value> &visitor) const = 0;
    virtual void accept(Visitor<void> &visitor) const = 0;
};
using ExprPtr = std::shared_ptr<BaseExpr const>;

struct Binary : public BaseExpr, public std::enable_shared_from_this<Binary> {
    const ExprPtr left;
    const ExprPtr right;
    const Token op;

    Binary(ExprPtr left, ExprPtr right, Token op)
        : left(left), right(right), op(op) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Grouping : public BaseExpr,
                  public std::enable_shared_from_this<Grouping> {
    const ExprPtr inner;

    Grouping(ExprPtr expr) : inner(expr) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Unary : public BaseExpr, public std::enable_shared_from_this<Unary> {
    const Token op;
    const ExprPtr right;

    Unary(Token op, ExprPtr right) : op(op), right(right) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Literal : public BaseExpr, public std::enable_shared_from_this<Literal> {
    const LiteralValue value;

    Literal(LiteralValue value) : value(value) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Variable : public BaseExpr,
                  public std::enable_shared_from_this<Variable> {
    const Token name;

    Variable(Token name) : name(name) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Assign : public BaseExpr, public std::enable_shared_from_this<Assign> {
    const Token name;
    const ExprPtr value;

    Assign(Token name, ExprPtr value) : name(name), value(value) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Logical : public BaseExpr, public std::enable_shared_from_this<Logical> {
    const ExprPtr left;
    const ExprPtr right;
    const Token op;

    Logical(ExprPtr left, ExprPtr right, Token op)
        : left(left), right(right), op(op) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Call : public BaseExpr, public std::enable_shared_from_this<Call> {
    const ExprPtr callee;
    const Token paren;
    const std::vector<ExprPtr> arguments;

    Call(ExprPtr callee, Token paren, std::vector<ExprPtr> arguments)
        : callee(callee), paren(paren), arguments(std::move(arguments)) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Get : public BaseExpr, public std::enable_shared_from_this<Get> {
    const ExprPtr object;
    const Token name;

    Get(ExprPtr object, Token name) : object(object), name(name) {}
    DEFINE_NODE_ACCEPT_METHOD(Value)
    DEFINE_NODE_ACCEPT_METHOD(void)
};

} // namespace Expr

#undef DEFINE_NODE_ACCEPT_METHOD
#endif // !CLOXPP_EXPR_H
