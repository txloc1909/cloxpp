#ifndef CLOXPP_DEBUG_H
#define CLOXPP_DEBUG_H

#include "chunk.hpp"

void disassembleChunk(const Chunk *chunk, const char *name);
int disassembleInstruction(const Chunk *chunk, int offset);

#endif // !CLOXPP_DEBUG_H
