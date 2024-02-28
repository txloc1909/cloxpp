#ifndef CLOXPP_CHUNK_H
#define CLOXPP_CHUNK_H

#include <cstdint>

typedef enum {
    OP_RETURN,
} OpCode;

class Chunk {
public:
    Chunk();
    ~Chunk();
    void write(uint8_t byte);

private:
    int count;
    int capacity;
    uint8_t *code;
};

#endif // !CLOXPP_CHUNK_H
