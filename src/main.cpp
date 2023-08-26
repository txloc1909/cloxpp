#include <cstdlib>
#include <iostream>

#include "treewalkinterpreter.hpp"


int main(int argc, char *argv[]) {
    TreewalkInterpreter interpreter;
    if (argc > 2) {
        std::cerr << "Usage: jlox [script]";
        std::exit(64);
    } else if (argc == 2) {
        interpreter.runFile(argv[1]);
    } else {
        interpreter.runPrompt();
    }
}
