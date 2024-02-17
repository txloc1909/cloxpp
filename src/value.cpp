#include "value.hpp"
#include "lox_callable.hpp"
#include "lox_class.hpp"

struct ToStringVisitor {
    std::string operator()(std::monostate) { return "nil"; }
    std::string operator()(const std::string &str) { return str; };
    std::string operator()(double d) { return std::to_string(d); }
    std::string operator()(bool b) { return b ? "true" : "false"; }
    std::string operator()(const std::shared_ptr<LoxCallable> &callable) {
        return callable->toString();
    }
    std::string operator()(const std::shared_ptr<LoxInstance> &instance) {
        return instance->toString();
    }
};

std::string stringify(const Literal &value) {
    return std::visit(ToStringVisitor(), value);
}

std::string stringify(const Value &value) {
    return std::visit(ToStringVisitor(), value);
}

std::ostream &operator<<(std::ostream &os, const Literal &literal) {
    os << stringify(literal);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Value &value) {
    os << stringify(value);
    return os;
}
