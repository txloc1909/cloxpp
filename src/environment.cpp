#include "environment.hpp"
#include "runtime_error.hpp"
#include <cassert>

Environment *Environment::ancestor(int distance) const {
    if (distance == 0) {
        return const_cast<Environment *>(this);
    } else if (distance == 1) {
        return enclosing.get();
    } else {
        assert(enclosing);
        return enclosing->ancestor(distance - 1);
    }
}

void Environment::define(std::string name, Value value) {
    assert(values.find(name) == values.end());
    values[name] = value;
}

void Environment::assign(const Token &name, Value value) {
    if (values.find(name.lexeme) != values.end()) {
        values.at(name.lexeme) = value;
        return;
    }

    if (enclosing) {
        enclosing->assign(name, value);
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assignAt(int distance, const Token &name, Value value) {
    auto *target_env = ancestor(distance);
    auto &target_values = target_env->values;
    assert(target_values.find(name.lexeme) != target_values.end());
    target_values[name.lexeme] = value;
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

Value Environment::getAt(int distance, const std::string &name) const {
    return ancestor(distance)->values.at(name);
}
