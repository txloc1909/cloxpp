#ifndef CLOXPP_STACK_VM_H
#define CLOXPP_STACK_VM_H

#include "vm.hpp"

namespace Clox {

class StackVM {
public:
    StackVM() = default;

    void runFile(const char *path);
    void runPrompt();

private:
    VM vm{};
};

} // namespace Clox

#endif // !CLOXPP_STACK_VM_H
