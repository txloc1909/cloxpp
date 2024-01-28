#ifndef CLOXPP_VALUE_H
#define CLOXPP_VALUE_H

#include <string>
#include <variant>

// monostate represents the nil value
using Literal = std::variant<std::monostate, std::string, double, bool>;

std::string stringify(const Literal &value);
std::ostream &operator<<(std::ostream &os, const Literal &literal);

#endif // !CLOXPP_VALUE_H
