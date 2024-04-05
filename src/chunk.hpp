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
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
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
