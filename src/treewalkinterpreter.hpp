#ifndef CLOXPP_TREEWALK_INTERPRETER_H
#define CLOXPP_TREEWALK_INTERPRETER_H

#include <string>

class TreewalkInterpreter {
    private:
        bool hadError = false;
        bool hadRuntimeError = false;

        void error(int line, const std::string& message);
        void report(int line, const std::string& where, const std::string& message);
    public:
        void run(std::string source);
        void runFile(const char* path);
        void runPrompt();
};

#endif // !CLOXPP_TREEWALK_INTERPRETER_H
