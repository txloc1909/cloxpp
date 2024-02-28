#include "chunk.hpp"
#include "debug.hpp"

int main() {
    Chunk chunk{};
    chunk.write(OP_RETURN);
    chunk.write(OP_RETURN);
    chunk.write(OP_RETURN);
    chunk.write(OP_RETURN);
    chunk.write(OP_RETURN);
    chunk.write(OP_RETURN);

    disassembleChunk(&chunk, "test chunk");
    return 0;
}
