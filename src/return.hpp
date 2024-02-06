#ifndef CLOXPP_RETURN_H
#define CLOXPP_RETURN_H

#include <exception>

#include "value.hpp"

class RuntimeReturn : std::exception {
public:
    Value value;
    RuntimeReturn(Value value) : value(value) {}
};

#endif // !CLOXPP_RETURN_H
