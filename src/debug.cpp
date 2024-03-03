#include <cstdio>

#include "debug.hpp"

namespace Clox {

void disassembleChunk(const Chunk *chunk, const char *name) {
    std::printf("=== %s ===\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int constantInstruction(const char *name, const Chunk *chunk,
                               int offset) {
    uint8_t constant = chunk->code[offset + 1];

    std::printf("%-16s %4d '", name, constant);
    std::printf("%g", chunk->constants.values[constant]); // temporarily
    std::printf("'\n");

    return offset + 2;
}

static int simpleInstruction(const char *name, int offset) {
    std::printf("%s\n", name);
    return offset + 1;
}

int disassembleInstruction(const Chunk *chunk, int offset) {
    std::printf("%04d ", offset);

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    default:
        std::printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}

} // namespace Clox
