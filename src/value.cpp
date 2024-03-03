#include "value.hpp"
#include "lox_class.hpp"
#include "memory.hpp"

namespace Jlox {

struct ToStringVisitor {
    std::string operator()(std::monostate) { return "nil"; }
    std::string operator()(const std::string &str) { return str; };
    std::string operator()(double d) {
        auto s = std::to_string(d);
        auto dotPos = s.find_last_of('.');
        auto lastNonZero = s.find_last_not_of('0');
        if (lastNonZero != std::string::npos) {
            s.erase(lastNonZero == dotPos ? lastNonZero : lastNonZero + 1);
        }

        return s;
    }
    std::string operator()(bool b) { return b ? "true" : "false"; }
    std::string operator()(const LoxCallablePtr &callable) {
        return callable->toString();
    }
    std::string operator()(const LoxInstancePtr &instance) {
        return instance->toString();
    }
};

std::string stringify(const Literal &value) {
    return std::visit(ToStringVisitor(), value);
}

std::string stringify(const Value &value) {
    return std::visit(ToStringVisitor(), value);
}

std::ostream &operator<<(std::ostream &os, const Literal &literal) {
    os << stringify(literal);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Value &value) {
    os << stringify(value);
    return os;
}

} // namespace Jlox

namespace Clox {

ValueArray::ValueArray() : capacity(0), count(0), values(nullptr) {}

ValueArray::~ValueArray() {
    FREE_ARRAY(Value, values, capacity);
    capacity = 0;
    count = 0;
}

void ValueArray::write(Value value) {
    if (capacity < count + 1) {
        int oldCapacity = capacity;
        capacity = GROW_CAPACITY(oldCapacity);
        values = GROW_ARRAY(Value, values, oldCapacity, capacity);
    }

    values[count] = value;
    count++;
}

} // namespace Clox
