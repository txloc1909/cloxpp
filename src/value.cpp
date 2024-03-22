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

struct ToStringVisitor {
    std::string operator()(std::monostate) { return "nil"; }
    std::string operator()(double d) {
        char buffer[13]; // %g directive writes between 1 and 13 bytes
        std::sprintf(buffer, "%g", d);
        return std::string(buffer);
    }
    std::string operator()(bool b) { return b ? "true" : "false"; }
};

std::ostream &operator<<(std::ostream &os, const Value &value) {
    os << std::visit(ToStringVisitor(), value);
    return os;
}

} // namespace Clox
