#ifndef CLOXPP_ENVIRONMENT_H
#define CLOXPP_ENVIRONMENT_H

#include <string>
#include <unordered_map>

#include "token.hpp"
#include "value.hpp"

class Environment {
private:
    std::unordered_map<std::string, Value> values;

public:
    void define(std::string name, Value value);
    Value get(Token name) const;
};

#endif // !CLOXPP_ENVIRONMENT_H
