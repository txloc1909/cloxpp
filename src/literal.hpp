#ifndef CLOXPP_LITERAL_H
#define CLOXPP_LITERAL_H

#include <ostream>
#include <optional>
#include <variant>

using Literal = std::optional<std::variant<std::string, double, bool>>;

template<typename T, typename... Ts>
std::ostream& operator<<(std::ostream& os, const std::optional<std::variant<T, Ts...>>& v) {
    if (v.has_value()) {
        std::visit([&os](auto&& arg) { os << arg; }, v.value());
    }
    return os;
}

#endif // !CLOXPP_LITERAL_H
