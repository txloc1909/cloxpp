#ifndef CLOXPP_CHUNK_H
#define CLOXPP_CHUNK_H

#include <cstdint>

namespace Clox {

enum OpCode {
    OP_RETURN,
};

struct Chunk {
    int count;
    int capacity;
    uint8_t *code;

    Chunk();
    ~Chunk();
    void write(uint8_t byte);
};

} // namespace Clox
#endif // !CLOXPP_CHUNK_H
