#ifndef CLOXPP_VM_H
#define CLOXPP_VM_H

#include "chunk.hpp"
#include "table.hpp"

namespace Clox {

enum class InterpretResult {
    OK,
    COMPILE_ERROR,
    RUNTIME_ERROR,
};

class VM {
public:
    VM();
    ~VM();
    InterpretResult interpret(const std::string &source);

private:
    static constexpr std::size_t STACK_MAX = 256;

    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *stackTop;
    Table globals;

    InterpretResult run();

    void push(Value value);
    Value pop();
    Value peek(int distance);

    void resetStack();
    void runtimeError(const char *format, ...);
};

} // namespace Clox

#endif // !CLOXPP_VM_H
