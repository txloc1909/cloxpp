#include <cassert>
#include <chrono>
#include <cstddef>

#include "lox_function.hpp"
#include "return.hpp"

Value LoxFunction::call(Interpreter &interpreter,
                        const std::vector<Value> &arguments) {
    auto environment = std::make_shared<Environment>(closure);

    assert(declaration->params.size() == arguments.size());
    for (size_t i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params.at(i).lexeme, arguments.at(i));
    }

    try {
        interpreter.executeBlock(declaration->body, environment);
    } catch (const RuntimeReturn &returnValue) {
        return returnValue.value;
    }

    return {};
}

std::size_t LoxFunction::arity() const { return declaration->params.size(); }

std::string LoxFunction::toString() const {
    return "<fn " + declaration->name.lexeme + ">";
}

Value NativeClock::call(Interpreter &interpreter,
                        const std::vector<Value> &arguments) {
    return static_cast<double>(
               std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::high_resolution_clock::now().time_since_epoch())
                   .count()) /
           1000.0;
}

std::size_t NativeClock::arity() const { return 0; }

std::string NativeClock::toString() const { return "<native fn>"; }
