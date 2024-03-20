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
    std::printf("%g", std::get<double>(
                          chunk->constants.values[constant])); // temporarily
    std::printf("'\n");

    return offset + 2;
}

static int simpleInstruction(const char *name, int offset) {
    std::printf("%s\n", name);
    return offset + 1;
}

int disassembleInstruction(const Chunk *chunk, int offset) {
    std::printf("%04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        std::printf("   | ");
    } else {
        std::printf("%4d ", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);
    case OP_ADD:
        return simpleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
        return simpleInstruction("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    default:
        std::printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}

} // namespace Clox
