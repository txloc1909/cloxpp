#ifndef CLOXPP_TREEWALK_INTERPRETER_H
#define CLOXPP_TREEWALK_INTERPRETER_H

#include <memory>
#include <string>

#include "error_handler.hpp"
#include "interpreter.hpp"

class TreewalkInterpreter {
public:
    TreewalkInterpreter();

    void run(const std::string &source);
    void runFile(const char *path);
    void runPrompt();

private:
    std::unique_ptr<ErrorHandler> errorHandler;
    Interpreter interpreter;
};

#endif // !CLOXPP_TREEWALK_INTERPRETER_H
