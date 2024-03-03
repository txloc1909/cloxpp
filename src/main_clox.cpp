#include "chunk.hpp"
#include "debug.hpp"
#include "vm.hpp"

int main() {
    using namespace Clox;

    VM vm{};

    Chunk chunk{};

    int constant = chunk.addConstant(1.2);
    chunk.write(OP_CONSTANT, 123);
    chunk.write(constant, 123);

    chunk.write(OP_RETURN, 123);

    disassembleChunk(&chunk, "test chunk");

    vm.interpret(&chunk);
    return 0;
}
