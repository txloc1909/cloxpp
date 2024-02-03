#include "lox_function.hpp"

Value LoxFunction::call(Interpreter &interpreter,
                        const std::vector<Value> &arguments) {
    // TODO: implement this method properly
    return {};
}

std::size_t LoxFunction::arity() const {
    // TODO: implement this method properly
    return 0;
}

std::string LoxFunction::toString() const {
    // TODO: implement this method properly
    return "<fn>";
}
