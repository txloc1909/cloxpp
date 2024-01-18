#ifndef CLOXPP_LITERAL_H
#define CLOXPP_LITERAL_H

#include <optional>
#include <ostream>
#include <variant>

using Literal = std::optional<std::variant<std::string, double, bool>>;

inline std::string stringify(const Literal &literal) {
    struct ToStringVisitor {
        std::string operator()(const std::string &str) const { return str; };
        std::string operator()(double d) { return std::to_string(d); }
        std::string operator()(bool b) { return b ? "true" : "false"; }
    };

    if (literal.has_value()) {
        return std::visit(ToStringVisitor(), literal.value());
    } else {
        return "null";
    }
}

template <typename T, typename... Ts>
std::ostream &operator<<(std::ostream &os,
                         const std::optional<std::variant<T, Ts...>> &v) {
    if (v.has_value()) {
        std::visit([&os](auto &&arg) { os << arg; }, v.value());
    } else {
        os << "null";
    }
    return os;
}

#endif // !CLOXPP_LITERAL_H
