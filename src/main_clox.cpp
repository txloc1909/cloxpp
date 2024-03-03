#include "chunk.hpp"
#include "debug.hpp"

int main() {
    using namespace Clox;

    Chunk chunk{};

    int constant = chunk.addConstant(1.2);
    chunk.write(OP_CONSTANT, 123);
    chunk.write(constant, 123);

    chunk.write(OP_RETURN, 123);

    disassembleChunk(&chunk, "test chunk");
    return 0;
}
