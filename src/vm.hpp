#ifndef CLOXPP_VM_H
#define CLOXPP_VM_H

#include "chunk.hpp"

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
    void runFile(const char *path);
    void repl();
    InterpretResult interpret(const std::string &source);
    InterpretResult run();

    void push(Value value);
    Value pop();
    Value peek(int distance);

private:
    static constexpr std::size_t STACK_MAX = 256;

    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *stackTop;

    void resetStack();
    void runtimeError(const char *format, ...);
};

} // namespace Clox

#endif // !CLOXPP_VM_H
