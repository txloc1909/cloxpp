#include "value.hpp"

struct ToStringVisitor {
    std::string operator()(std::monostate) { return "nil"; }
    std::string operator()(const std::string &str) { return str; };
    std::string operator()(double d) { return std::to_string(d); }
    std::string operator()(bool b) { return b ? "true" : "false"; }
};

std::string stringify(const Literal &value) {
    return std::visit(ToStringVisitor(), value);
}

std::ostream &operator<<(std::ostream &os, const Literal &literal) {
    os << stringify(literal);
    return os;
}

bool Value::isTruthy() const {
    if (std::holds_alternative<std::monostate>(*this))
        return false;

    if (std::holds_alternative<bool>(*this))
        return std::get<bool>(*this);

    return true;
}

Value valueFromLiteral(const Literal &literal) {
    // this look tedious, but works for now
    if (std::holds_alternative<std::string>(literal)) {
        return std::get<std::string>(literal);
    } else if (std::holds_alternative<double>(literal)) {
        return std::get<double>(literal);
    } else if (std::holds_alternative<bool>(literal)) {
        return std::get<bool>(literal);
    } else {
        return {};
    }
}
