#include "environment.hpp"
#include "runtime_error.hpp"

void Environment::define(std::string name, Value value) {
    values[name] = value;
}

Value Environment::get(Token name) const {
    if (values.find(name.lexeme) != values.end()) {
        return values.at(name.lexeme);
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}
