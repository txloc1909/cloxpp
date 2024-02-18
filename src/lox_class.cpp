#include "lox_class.hpp"
#include "runtime_error.hpp"

Value LoxClass::call(Interpreter & /*interpreter*/,
                     const std::vector<Value> & /*arguments*/) {
    return std::make_shared<LoxInstance>(this);
}

std::size_t LoxClass::arity() const { return 0; }

std::string LoxClass::toString() const { return name; }

const std::string &LoxClass::getName() const { return name; }

LoxFunctionPtr LoxClass::findMethod(const std::string &name) const {
    if (methods.find(name) != methods.end()) {
        return methods.at(name);
    }

    return nullptr;
}

Value LoxInstance::get(const Token &name) const {
    if (fields.find(name.lexeme) != fields.end()) {
        return fields.at(name.lexeme);
    }

    LoxFunctionPtr method = klass->findMethod(name.lexeme);
    if (method) {
        return method->bind(
            const_cast<LoxInstance *>(this)->shared_from_this());
    }

    throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void LoxInstance::set(const Token &name, Value value) {
    fields[name.lexeme] = value;
}

std::string LoxInstance::toString() const {
    return klass->getName() + " instance";
}
