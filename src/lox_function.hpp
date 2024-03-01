#ifndef CLOXPP_LOX_FUNCTION_H
#define CLOXPP_LOX_FUNCTION_H

#include "lox_callable.hpp"
#include "stmt.hpp"

namespace Jlox {

enum class FunctionType {
    NONE,
    FUNCTION,
    METHOD,
    INITIALIZER,
};
const char *functionTypeToString(FunctionType type);

class LoxFunction;
using LoxFunctionPtr = std::shared_ptr<LoxFunction>;

class LoxFunction : public LoxCallable {
private:
    const Stmt::FunctionPtr declaration;
    const EnvironmentPtr closure;
    const bool isInitializer;

public:
    LoxFunction(Stmt::FunctionPtr declaration, EnvironmentPtr closure)
        : declaration(declaration), closure(closure), isInitializer(false) {}

    LoxFunction(Stmt::FunctionPtr declaration, EnvironmentPtr closure,
                bool isInitializer)
        : declaration(declaration), closure(closure),
          isInitializer(isInitializer) {}

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

} // namespace Jlox
#endif // !CLOXPP_LOX_FUNCTION_H
