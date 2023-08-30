#ifndef CLOXPP_TREEWALK_INTERPRETER_H
#define CLOXPP_TREEWALK_INTERPRETER_H

#include <memory>
#include <string>

#include "error_handler.hpp"

class TreewalkInterpreter {
    public:
        TreewalkInterpreter();

        void run(std::string source);
        void runFile(const char* path);
        void runPrompt();

    private:
        std::shared_ptr<ErrorHandler> errorHandler;
};

#endif // !CLOXPP_TREEWALK_INTERPRETER_H
