#ifndef CLOXPP_CHUNK_H
#define CLOXPP_CHUNK_H

#include <cstdint>

#include "value.hpp"

namespace Clox {

enum OpCode {
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
};

struct Chunk {
    int count;
    int capacity;
    uint8_t *code;
    int *lines;
    ValueArray constants;

    Chunk();
    ~Chunk();
    void write(uint8_t byte, int line);
    int addConstant(Value value);
};

} // namespace Clox
#endif // !CLOXPP_CHUNK_H
