#ifndef CLOXPP_CHUNK_H
#define CLOXPP_CHUNK_H

#include <cstdint>

#include "value.hpp"

namespace Clox {

enum OpCode {
    OP_RETURN,
};

struct Chunk {
    int count;
    int capacity;
    uint8_t *code;
    ValueArray constants;

    Chunk();
    ~Chunk();
    void write(uint8_t byte);
    int addConstant(Value value);
};

} // namespace Clox
#endif // !CLOXPP_CHUNK_H
