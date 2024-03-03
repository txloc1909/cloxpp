#include "chunk.hpp"
#include "debug.hpp"

int main() {
    using namespace Clox;

    Chunk chunk{};

    int constant = chunk.addConstant(1.2);
    chunk.write(OP_CONSTANT);
    chunk.write(constant);

    chunk.write(OP_RETURN);

    disassembleChunk(&chunk, "test chunk");
    return 0;
}
