#include "value.hpp"
#include "lox_class.hpp"
#include "memory.hpp"

namespace Jlox {

struct ToStringVisitor {
    std::string operator()(std::monostate) { return "nil"; }
    std::string operator()(const std::string &str) { return str; };
    std::string operator()(double d) {
        char buffer[13]; // %g directive writes between 1 and 13 bytes
        std::sprintf(buffer, "%g", d);
        return std::string(buffer);
    }
    std::string operator()(bool b) { return b ? "true" : "false"; }
    std::string operator()(const LoxCallablePtr &callable) {
        return callable->toString();
    }
    std::string operator()(const LoxInstancePtr &instance) {
        return instance->toString();
    }
};

std::ostream &operator<<(std::ostream &os, const Value &value) {
    os << std::visit(ToStringVisitor(), value);
    return os;
}

} // namespace Jlox

namespace Clox {

ValueArray::ValueArray() : capacity(0), count(0), values(nullptr) {}

ValueArray::~ValueArray() {
    Allocator::freeArray<Value>(values, capacity);
    capacity = 0;
    count = 0;
}

void ValueArray::write(Value value) {
    if (capacity < count + 1) {
        int oldCapacity = capacity;
        capacity = Allocator::growCapacity(oldCapacity);
        values = Allocator::growArray<Value>(values, oldCapacity, capacity);
    }

    values[count] = value;
    count++;
}

struct ToStringVisitor {
    std::string operator()(Nil) { return "nil"; }
    std::string operator()(Number num) {
        char buffer[13]; // %g directive writes between 1 and 13 bytes
        std::sprintf(buffer, "%g", num);
        return std::string(buffer);
    }
    std::string operator()(bool b) { return b ? "true" : "false"; }
    std::string operator()(const ObjString *str) {
        return std::string(str->data());
    }

    std::string operator()(const ObjFunction *func) {
        return "<fn " + std::string(func->getName()) + ">";
    }
};

struct ValueEquality {
    bool operator()(Nil, Nil) const { return true; }

    template <typename T>
    bool operator()(T a, T b) const {
        return a == b;
    }

    template <typename T, typename U,
              typename = std::enable_if_t<!std::is_same_v<T, U>>>
    bool operator()(T /*a*/, U /*b*/) const {
        return false;
    }
};

bool Value::operator==(const Value &other) const {
    return std::visit(ValueEquality(), *this, other);
}

bool Value::isFalsey() const {
    return isType<Nil>() || (isType<bool>() && !asType<bool>());
}

std::ostream &operator<<(std::ostream &os, const Value &value) {
    os << std::visit(ToStringVisitor(), value);
    return os;
}

} // namespace Clox
