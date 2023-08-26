#include <string>

class TreewalkInterpreter {
    private:
        bool hadError = false;
        bool hadRuntimeError = false;

        void error(int line, std::string message);
        void report(int line, std::string where, std::string message);
    public:
        void run(std::string source);
        void runFile(const char* path);
        void runPrompt();
};
