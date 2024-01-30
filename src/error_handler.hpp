#ifndef CLOXPP_ERROR_HANDLER_H
#define CLOXPP_ERROR_HANDLER_H

#include "runtime_error.hpp"
#include "token.hpp"

struct ErrorHandler {
    bool hadError;
    bool hadRuntimeError;

    ErrorHandler() : hadError(false), hadRuntimeError(false) {}
    void error(int line, const char *message);
    void error(Token token, const char *message);
    void report(int line, const std::string &where, const char *message);
    void runtimeError(const RuntimeError &error);
};

#endif // !CLOXPP_ERROR_HANDLER_H
