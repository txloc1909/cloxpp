#ifndef CLOXPP_STMT_H
#define CLOXPP_STMT_H

#include <memory>
#include <vector>

#include "expr.hpp"

#define DEFINE_NODE_ACCEPT_METHOD(ReturnType)                                  \
    ReturnType accept(Visitor<ReturnType> &visitor) const override {           \
        return visitor.visit(shared_from_this());                              \
    }

namespace Stmt {

using ExprNodePtr = Expr::ExprPtr;
using ExprVariablePtr = Expr::VariablePtr;

struct Expr;
struct Print;
struct Var;
struct Block;
struct If;
struct While;
struct Function;
struct Return;
struct Class;

using ExprPtr = std::shared_ptr<Expr const>;
using PrintPtr = std::shared_ptr<Print const>;
using VarPtr = std::shared_ptr<Var const>;
using BlockPtr = std::shared_ptr<Block const>;
using IfPtr = std::shared_ptr<If const>;
using WhilePtr = std::shared_ptr<While const>;
using FunctionPtr = std::shared_ptr<Function const>;
using ReturnPtr = std::shared_ptr<Return const>;
using ClassPtr = std::shared_ptr<Class const>;

template <typename R>
class Visitor {
public:
    virtual R visit(ExprPtr expr) = 0;
    virtual R visit(PrintPtr expr) = 0;
    virtual R visit(VarPtr expr) = 0;
    virtual R visit(BlockPtr expr) = 0;
    virtual R visit(IfPtr expr) = 0;
    virtual R visit(WhilePtr expr) = 0;
    virtual R visit(FunctionPtr expr) = 0;
    virtual R visit(ReturnPtr expr) = 0;
    virtual R visit(ClassPtr expr) = 0;

    virtual ~Visitor() = default;
};

struct BaseStmt {
    virtual ~BaseStmt() = default;
    virtual void accept(Visitor<void> &visitor) const = 0;
};
using StmtPtr = std::shared_ptr<BaseStmt const>;

struct Expr : public BaseStmt, public std::enable_shared_from_this<Expr> {
    const ExprNodePtr expr;

    Expr(ExprNodePtr expr) : expr(expr) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Print : public BaseStmt, public std::enable_shared_from_this<Print> {
    const ExprNodePtr expr;

    Print(ExprNodePtr expr) : expr(expr) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Var : public BaseStmt, public std::enable_shared_from_this<Var> {
    const Token name;
    const ExprNodePtr initializer;

    Var(Token name, ExprNodePtr initializer)
        : name(name), initializer(initializer) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Block : public BaseStmt, public std::enable_shared_from_this<Block> {
    const std::vector<StmtPtr> statements;

    Block(std::vector<StmtPtr> statements)
        : statements(std::move(statements)) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct If : public BaseStmt, public std::enable_shared_from_this<If> {
    const ExprNodePtr condition;
    const StmtPtr thenBranch;
    const StmtPtr elseBranch;

    If(ExprNodePtr condition, StmtPtr thenBranch, StmtPtr elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
    }
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct While : public BaseStmt, public std::enable_shared_from_this<While> {
    const ExprNodePtr condition;
    const StmtPtr body;

    While(ExprNodePtr condition, StmtPtr body)
        : condition(condition), body(body) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Function : public BaseStmt,
                  public std::enable_shared_from_this<Function> {
    const Token name;
    const std::vector<Token> params;
    const std::vector<StmtPtr> body;

    Function(Token name, std::vector<Token> params, std::vector<StmtPtr> body)
        : name(name), params(std::move(params)), body(std::move(body)){};
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Return : public BaseStmt, public std::enable_shared_from_this<Return> {
    const Token keyword;
    const ExprNodePtr value;

    Return(Token keyword, ExprNodePtr value) : keyword(keyword), value(value) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

struct Class : public BaseStmt, public std::enable_shared_from_this<Class> {
    const Token name;
    const ExprVariablePtr superclass;
    const std::vector<FunctionPtr> methods;

    Class(Token name, ExprVariablePtr superclass,
          std::vector<FunctionPtr> methods)
        : name(name), superclass(superclass), methods(std::move(methods)) {}
    DEFINE_NODE_ACCEPT_METHOD(void)
};

} // namespace Stmt

#undef DEFINE_NODE_ACCEPT_METHOD
#endif // !CLOXPP_STMT_H
