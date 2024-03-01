#ifndef CLOXPP_VALUE_H
#define CLOXPP_VALUE_H

#include <memory>
#include <string>

#include "concatenator.hpp"

namespace Jlox {

// monostate represents the nil value
using Literal = std::variant<std::monostate, std::string, double, bool>;

// forward declare some types here, to be included in Value
class LoxCallable;
class LoxInstance;
using LoxCallablePtr = std::shared_ptr<LoxCallable>;
using LoxInstancePtr = std::shared_ptr<LoxInstance>;

using Value = concatenator<Literal, LoxCallablePtr, LoxInstancePtr>::type;

std::string stringify(const Literal &value);
std::string stringify(const Value &value);

std::ostream &operator<<(std::ostream &os, const Literal &literal);
std::ostream &operator<<(std::ostream &os, const Value &value);

} // namespace Jlox
#endif // !CLOXPP_VALUE_H
