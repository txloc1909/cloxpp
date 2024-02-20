#include <cassert>
#include <chrono>
#include <memory>

#include "lox_class.hpp"
#include "lox_function.hpp"
#include "return.hpp"

const char *functionTypeToString(FunctionType type) {
    switch (type) {
    case FunctionType::NONE:
        return "";
    case FunctionType::FUNCTION:
        return "function";
    case FunctionType::METHOD:
        return "method";
    case FunctionType::INITIALIZER:
        return "initializer";
    default:
        throw std::runtime_error("Unreachable!");
    }
}

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
        if (isInitializer) {
            return closure->getAt(0, "this");
        }
        return returnValue.value;
    }

    if (isInitializer) {
        return closure->getAt(0, "this");
    }

    return {};
}

std::size_t LoxFunction::arity() const { return declaration->params.size(); }

std::string LoxFunction::toString() const {
    return "<fn " + declaration->name.lexeme + ">";
}

LoxFunctionPtr LoxFunction::bind(LoxInstancePtr instance) {
    auto environment = std::make_shared<Environment>(closure);
    environment->define("this", instance);
    return std::make_shared<LoxFunction>(declaration, environment,
                                         isInitializer);
}

Value NativeClock::call(Interpreter & /*interpreter*/,
                        const std::vector<Value> & /*arguments*/) {
    return static_cast<double>(
               std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::high_resolution_clock::now().time_since_epoch())
                   .count()) /
           1000.0;
}

std::size_t NativeClock::arity() const { return 0; }

std::string NativeClock::toString() const { return "<native fn>"; }
