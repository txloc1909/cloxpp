#ifndef CLOXPP_CHUNK_H
#define CLOXPP_CHUNK_H

#include <cstdint>

typedef enum {
    OP_RETURN,
} OpCode;

struct Chunk {
    int count;
    int capacity;
    uint8_t *code;

    Chunk();
    ~Chunk();
    void write(uint8_t byte);
};

#endif // !CLOXPP_CHUNK_H
