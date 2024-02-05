#include <chrono>

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
