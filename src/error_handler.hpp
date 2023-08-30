#ifndef CLOXPP_ERROR_HANDLER_H
#define CLOXPP_ERROR_HANDLER_H

#include <string>

typedef struct ErrorHandler {
    bool hadError;
    bool hadRuntimeError;

    ErrorHandler() : hadError(false), hadRuntimeError(false) {}
    void error(int line, const char* message);
    void report(int line, const char* where, const char* message);
} ErrorHandler;

#endif // !CLOXPP_ERROR_HANDLER_H
