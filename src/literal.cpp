#include "literal.hpp"

std::ostream& operator<<(std::ostream &os, const Literal& literal) {
    if (literal.has_value()) {
        std::visit([&os](auto&& arg) { os << arg; }, literal.value());
    }
    return os;
}
