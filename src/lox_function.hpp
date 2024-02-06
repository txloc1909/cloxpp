#ifndef CLOXPP_LOX_FUNCTION_H
#define CLOXPP_LOX_FUNCTION_H

#include "lox_callable.hpp"

class LoxFunction : public LoxCallable {
private:
    const std::shared_ptr<Stmt::Function> declaration;
    Environment *closure;

public:
    LoxFunction(std::shared_ptr<Stmt::Function> declaration,
                Environment *closure)
        : declaration(declaration), closure(closure) {}

    Value call(Interpreter &interpreter,
               const std::vector<Value> &arguments) override;
    std::size_t arity() const override;
    std::string toString() const override;
};

class NativeClock : public LoxCallable {
public:
    Value call(Interpreter &interpreter,
               const std::vector<Value> &arguments) override;
    std::size_t arity() const override;
    std::string toString() const override;
};

#endif // !CLOXPP_LOX_FUNCTION_H
