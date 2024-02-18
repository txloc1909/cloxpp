#ifndef CLOXPP_VALUE_H
#define CLOXPP_VALUE_H

#include <memory>
#include <string>
#include <variant>

template <typename T, typename... Args>
struct concatenator;

template <typename... Args0, typename... Args1>
struct concatenator<std::variant<Args0...>, Args1...> {
    using type = std::variant<Args0..., Args1...>;
};

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

#endif // !CLOXPP_VALUE_H
