#include "lox_class.hpp"
#include "runtime_error.hpp"

namespace Jlox {

Value LoxClass::call(Interpreter &interpreter,
                     const std::vector<Value> &arguments) {
    auto instance = std::make_shared<LoxInstance>(this);
    if (auto initializer = findMethod("init")) {
        initializer->bind(instance)->call(interpreter, arguments);
    }
    return instance;
}

std::size_t LoxClass::arity() const {
    auto initializer = findMethod("init");
    return initializer ? initializer->arity() : 0;
}

std::string LoxClass::toString() const { return name; }

const std::string &LoxClass::getName() const { return name; }

LoxFunctionPtr LoxClass::findMethod(const std::string &name) const {
    if (methods.find(name) != methods.end()) {
        return methods.at(name);
    }

    if (superclass) {
        return superclass->findMethod(name);
    }

    return nullptr;
}

Value LoxInstance::get(const Token &name) const {
    auto attrName = name.getLexemeString();
    if (fields.find(attrName) != fields.end()) {
        return fields.at(attrName);
    }

    LoxFunctionPtr method = klass->findMethod(attrName);
    if (method) {
        return method->bind(
            const_cast<LoxInstance *>(this)->shared_from_this());
    }

    throw RuntimeError(name, "Undefined property '" + attrName + "'.");
}

void LoxInstance::set(const Token &name, Value value) {
    fields[name.getLexemeString()] = value;
}

std::string LoxInstance::toString() const {
    return klass->getName() + " instance";
}
} // namespace Jlox
