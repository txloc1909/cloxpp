#ifndef CLOXPP_ENVIRONMENT_H
#define CLOXPP_ENVIRONMENT_H

#include <map>
#include <string>

#include "token.hpp"
#include "value.hpp"

class Environment;
using EnvironmentPtr = std::shared_ptr<Environment>;

class Environment {
private:
    std::map<std::string, Value> values;
    EnvironmentPtr enclosing = nullptr;

public:
    Environment() = default;
    Environment(EnvironmentPtr enclosing) : enclosing(enclosing) {}

    void define(std::string name, Value value);
    void assign(const Token &name, Value value);
    Value get(const Token &name) const;
};

#endif // !CLOXPP_ENVIRONMENT_H
