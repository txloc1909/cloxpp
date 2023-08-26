#include <string>

class TreewalkInterpreter {
    private:
        bool hadError = false;
        bool hadRuntimeError = false;
    public:
        void run(std::string source);
        void runFile(const char* path);
        void runPrompt();
};
