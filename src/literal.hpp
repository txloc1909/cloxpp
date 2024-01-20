#ifndef CLOXPP_LITERAL_H
#define CLOXPP_LITERAL_H

#include <string>
#include <variant>

using Literal = std::variant<std::monostate, std::string, double, bool>;

inline std::string stringifyLiteral(const Literal &literal) {
    struct ToStringVisitor {
        std::string operator()(const std::monostate &val) { return "null"; }
        std::string operator()(const std::string &str) const { return str; };
        std::string operator()(double d) { return std::to_string(d); }
        std::string operator()(bool b) { return b ? "true" : "false"; }
    };

    return std::visit(ToStringVisitor(), literal);
}

inline std::ostream &operator<<(std::ostream &os, const Literal &literal) {
    os << stringifyLiteral(literal);
    return os;
}
#endif // !CLOXPP_LITERAL_H
