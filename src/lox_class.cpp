#include "lox_class.hpp"

Value LoxClass::call(Interpreter & /*interpreter*/,
                     const std::vector<Value> & /*arguments*/) {
    return std::make_shared<LoxInstance>(this);
}

std::size_t LoxClass::arity() const { return 0; }

std::string LoxClass::toString() const { return name; }

const std::string &LoxClass::getName() const { return name; }

std::string LoxInstance::toString() const {
    return klass->getName() + " instance";
}
