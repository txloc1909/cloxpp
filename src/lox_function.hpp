#ifndef CLOXPP_LOX_FUNCTION_H
#define CLOXPP_LOX_FUNCTION_H

#include "lox_callable.hpp"
#include "stmt.hpp"

enum FunctionType {
    NONE,
    FUNCTION,
    METHOD,
};
const char *functionTypeToString(FunctionType type);

class LoxFunction;
using LoxFunctionPtr = std::shared_ptr<LoxFunction>;

class LoxFunction : public LoxCallable {
private:
    const Stmt::FunctionPtr declaration;
    const EnvironmentPtr closure;

public:
    LoxFunction(Stmt::FunctionPtr declaration, EnvironmentPtr closure)
        : declaration(declaration), closure(closure) {}

    Value call(Interpreter &interpreter,
               const std::vector<Value> &arguments) override;
    std::size_t arity() const override;
    std::string toString() const override;
    LoxFunctionPtr bind(LoxInstancePtr instance);
};

class NativeClock : public LoxCallable {
public:
    Value call(Interpreter &interpreter,
               const std::vector<Value> &arguments) override;
    std::size_t arity() const override;
    std::string toString() const override;
};

#endif // !CLOXPP_LOX_FUNCTION_H
