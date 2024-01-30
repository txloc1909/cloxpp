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
