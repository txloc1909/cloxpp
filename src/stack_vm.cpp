#include <iostream>

#include "stack_vm.hpp"
#include "utils.hpp"

namespace Clox {

void StackVM::runFile(const char *path) {
    auto result = vm.interpret(readFile(path));

    if (result == InterpretResult::COMPILE_ERROR)
        std::exit(65);
    if (result == InterpretResult::RUNTIME_ERROR)
        std::exit(70);
}

void StackVM::runPrompt() {
    std::string line;
    while (std::cin) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        vm.interpret(line);
    }
}

} // namespace Clox
