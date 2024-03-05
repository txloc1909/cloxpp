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

    constant = chunk.addConstant(3.4);
    chunk.write(OP_CONSTANT, 123);
    chunk.write(constant, 123);

    chunk.write(OP_ADD, 123);

    constant = chunk.addConstant(5.6);
    chunk.write(OP_CONSTANT, 123);
    chunk.write(constant, 123);

    chunk.write(OP_DIVIDE, 123);
    chunk.write(OP_NEGATE, 123);

    chunk.write(OP_RETURN, 123);

    disassembleChunk(&chunk, "test chunk");

    std::printf("=== execute ===\n");
    vm.interpret(&chunk);
    return 0;
}
