#ifndef CLOXPP_VALUE_H
#define CLOXPP_VALUE_H

#include <memory>
#include <string>
#include <variant>

#define UINT8_COUNT (UINT8_MAX + 1)

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

/*
 * Create my own Variant type, in order to rename
 * std::hold_alternative and std::get to better names
 */
template <typename... Types>
class Variant : public std::variant<Types...> {
public:
    using std::variant<Types...>::variant;

    template <typename T>
    bool isType() const {
        return std::holds_alternative<T>(*this);
    }

    template <typename T>
    T &asType() {
        return std::get<T>(*this);
    }

    template <typename T>
    const T &asType() const {
        return std::get<T>(*this);
    }
};

// forward declare Obj types
class Obj;
class ObjString;
class ObjFunction;

using Nil = std::monostate;
using Number = double;
using ValueTypes = Variant<Nil, Number, bool, ObjString *, ObjFunction *>;
class Value : public ValueTypes {
public:
    Value() : ValueTypes(std::in_place_type<Nil>) {}

    template <typename T>
    Value(T &&arg) : ValueTypes(std::forward<T>(arg)) {}

    bool operator==(const Value &other) const;
    bool isFalsey() const;
};

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

#endif // !CLOXPP_VALUE_H
