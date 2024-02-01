#include "environment.hpp"
#include "runtime_error.hpp"

void Environment::define(std::string name, Value value) {
    values[name] = value;
}

void Environment::assign(const Token &name, Value value) {
    if (values.find(name.lexeme) != values.end()) {
        values[name.lexeme] = value;
        return;
    }

    if (enclosing) {
        enclosing->assign(name, value);
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

Value Environment::get(const Token &name) const {
    if (values.find(name.lexeme) != values.end()) {
        return values.at(name.lexeme);
    }

    if (enclosing) {
        return enclosing->get(name);
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}
