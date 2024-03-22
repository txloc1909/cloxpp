#ifndef CLOXPP_VALUE_H
#define CLOXPP_VALUE_H

#include <memory>
#include <string>
#include <variant>

namespace Jlox {

// forward declare some types here, to be included in Value
class LoxCallable;
class LoxInstance;
using LoxCallablePtr = std::shared_ptr<LoxCallable>;
using LoxInstancePtr = std::shared_ptr<LoxInstance>;

// monostate represents the nil value
using Value = std::variant<std::monostate, std::string, double, bool,
                           LoxCallablePtr, LoxInstancePtr>;

std::ostream &operator<<(std::ostream &os, const Value &value);

} // namespace Jlox

namespace Clox {

using Nil = std::monostate;
using Value = std::variant<Nil, double, bool>;

std::ostream &operator<<(std::ostream &os, const Value &value);

struct ValueArray {
    int capacity;
    int count;
    Value *values;

    ValueArray();
    ~ValueArray();
    void write(Value value);
};

} // namespace Clox
//
#endif // !CLOXPP_VALUE_H
