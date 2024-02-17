#ifndef CLOXPP_LOX_CLASS_H
#define CLOXPP_LOX_CLASS_H

#include <string>

#include "lox_callable.hpp"

class LoxClass : public LoxCallable {
private:
    const std::string name;

public:
    LoxClass(std::string name) : name(name) {}

    const std::string &getName() const;
    Value call(Interpreter &interpreter,
               const std::vector<Value> &arguments) override;
    std::size_t arity() const override;
    std::string toString() const override;
};

class LoxInstance {
private:
    LoxClass *klass;

public:
    LoxInstance(LoxClass *klass) : klass(klass) {}
    std::string toString() const;
};

#endif // !CLOXPP_LOX_CLASS_H
