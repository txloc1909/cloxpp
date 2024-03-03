#ifndef CLOXPP_VM_H
#define CLOXPP_VM_H

#include "chunk.hpp"

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

private:
    Chunk *chunk;
    uint8_t *ip;
};

} // namespace Clox

#endif // !CLOXPP_VM_H
