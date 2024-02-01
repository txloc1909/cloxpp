#ifndef CLOXPP_ENVIRONMENT_H
#define CLOXPP_ENVIRONMENT_H

#include <string>
#include <unordered_map>

#include "token.hpp"
#include "value.hpp"

class Environment {
private:
    std::unordered_map<std::string, Value> values;
    Environment *enclosing = nullptr;

public:
    Environment() = default;
    Environment(Environment *enclosing) : enclosing(enclosing) {}

    void define(std::string name, Value value);
    void assign(const Token &name, Value value);
    Value get(const Token &name) const;
};

#endif // !CLOXPP_ENVIRONMENT_H
