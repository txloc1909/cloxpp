#include <cstdio>

#include "debug.hpp"

namespace Clox {

void disassembleChunk(const Chunk *chunk, const char *name) {
    std::printf("=== %s ===\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int simpleInstruction(const char *name, int offset) {
    std::printf("%s\n", name);
    return offset + 1;
}

int disassembleInstruction(const Chunk *chunk, int offset) {
    std::printf("%04d ", offset);

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    default:
        std::printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}

} // namespace Clox
