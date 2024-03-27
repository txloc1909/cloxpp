#include "chunk.hpp"
#include "memory.hpp"

namespace Clox {

Chunk::Chunk()
    : count(0), capacity(0), code(nullptr), lines(nullptr), constants({}) {}

Chunk::~Chunk() {
    Allocator::freeArray<uint8_t>(code, capacity);
    Allocator::freeArray<int>(lines, capacity);
    count = 0;
    capacity = 0;
}

void Chunk::write(uint8_t byte, int line) {
    if (capacity < count + 1) {
        auto oldCapacity = capacity;
        capacity = Allocator::growCapacity(oldCapacity);
        code = Allocator::growArray<uint8_t>(code, oldCapacity, capacity);
        lines = Allocator::growArray<int>(lines, oldCapacity, capacity);
    }

    code[count] = byte;
    lines[count] = line;
    count++;
}

int Chunk::addConstant(Value value) {
    constants.write(value);

    // return the index where the constant was appended
    return constants.count - 1;
}

} // namespace Clox
