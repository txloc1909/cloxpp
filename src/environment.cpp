#include "environment.hpp"
#include "runtime_error.hpp"
#include <cassert>

namespace Jlox {

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
    values[name] = value;
}

void Environment::assign(const Token &name, Value value) {
    auto target = name.getLexemeString();
    if (values.find(target) != values.end()) {
        values.at(target) = value;
        return;
    }

    if (enclosing) {
        enclosing->assign(name, value);
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + target + "'.");
}

void Environment::assignAt(int distance, const Token &name, Value value) {
    auto *targetEnv = ancestor(distance);
    auto &targetValues = targetEnv->values;
    auto targetName = name.getLexemeString();

    assert(targetValues.find(targetName) != targetValues.end());
    targetValues[targetName] = value;
}

Value Environment::get(const Token &name) const {
    auto target = name.getLexemeString();
    if (values.find(target) != values.end()) {
        return values.at(target);
    }

    if (enclosing) {
        return enclosing->get(name);
    }

    throw RuntimeError(name, "Undefined variable '" + target + "'.");
}

Value Environment::getAt(int distance, const std::string &name) const {
    return ancestor(distance)->values.at(name);
}

} // namespace Jlox
