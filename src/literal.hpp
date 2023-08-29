#ifndef CLOXPP_LITERAL_H
#define CLOXPP_LITERAL_H

#include <optional>
#include <variant>
#include <string>

using Literal = std::optional<std::variant<std::string, double, bool>>;
std::ostream& operator<<(std::ostream &os, const Literal& literal);

#endif // !CLOXPP_LITERAL_H
