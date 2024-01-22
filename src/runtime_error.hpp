#ifndef CLOXPP_RUNTIME_ERROR_H
#define CLOXPP_RUNTIME_ERROR_H

#include "token.hpp"

class RuntimeError : std::exception {
public:
    RuntimeError(Token token, std::string message)
        : token_(token), message_(message) {}

    const char *what() const noexcept override { return message_.c_str(); }
    Token getToken() const { return token_; }

private:
    const Token token_;
    const std::string message_;
};

#endif // !CLOXPP_RUNTIME_ERROR_H
