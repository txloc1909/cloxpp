#include <iostream>

#include "stack_vm.hpp"

int main(int argc, const char *argv[]) {
    using namespace Clox;

    StackVM vm{};
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [path]";
        std::exit(64);
    } else if (argc == 2) {
        vm.runFile(argv[1]);
    } else {
        vm.runPrompt();
    }

    return 0;
}
