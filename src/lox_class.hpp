#ifndef CLOXPP_LOX_CLASS_H
#define CLOXPP_LOX_CLASS_H

#include <string>

#include "lox_callable.hpp"
#include "lox_function.hpp"

class LoxClass : public LoxCallable {
private:
    const std::string name;
    std::unordered_map<std::string, LoxFunctionPtr> methods;

public:
    LoxClass(std::string name,
             std::unordered_map<std::string, LoxFunctionPtr> methods)
        : name(name), methods(std::move(methods)) {}

    const std::string &getName() const;
    LoxFunctionPtr findMethod(const std::string &name) const;
    Value call(Interpreter &interpreter,
               const std::vector<Value> &arguments) override;
    std::size_t arity() const override;
    std::string toString() const override;
};

class LoxInstance {
private:
    LoxClass *klass;
    std::unordered_map<std::string, Value> fields;

public:
    LoxInstance(LoxClass *klass) : klass(klass), fields({}) {}
    Value get(const Token &name) const;
    void set(const Token &name, Value value);
    std::string toString() const;
};

#endif // !CLOXPP_LOX_CLASS_H
