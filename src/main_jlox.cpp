#include <cstdlib>
#include <iostream>

#include "treewalk_interpreter.hpp"

int main(int argc, const char *argv[]) {
    using namespace Jlox;

    TreewalkInterpreter interpreter{};
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [script]";
        std::exit(64);
    } else if (argc == 2) {
        interpreter.runFile(argv[1]);
    } else {
        interpreter.runPrompt();
    }
}
