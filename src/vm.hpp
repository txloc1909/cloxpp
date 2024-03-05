#ifndef CLOXPP_VM_H
#define CLOXPP_VM_H

#include "chunk.hpp"

#define STACK_MAX 256

namespace Clox {

enum class InterpreteResult {
    OK,
    COMPILE_ERROR,
    RUNTIME_ERROR,
};

class VM {
public:
    VM();
    ~VM();
    InterpreteResult interpret(Chunk *chunk);
    InterpreteResult run();

    void push(Value value);
    Value pop();

private:
    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *stackTop;

    void resetStack();
};

} // namespace Clox

#endif // !CLOXPP_VM_H
