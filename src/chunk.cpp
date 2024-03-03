#include "chunk.hpp"
#include "memory.hpp"

namespace Clox {

Chunk::Chunk()
    : count(0), capacity(0), code(nullptr), lines(nullptr), constants({}) {}

Chunk::~Chunk() {
    FREE_ARRAY(uint8_t, code, capacity);
    FREE_ARRAY(int, lines, capacity);
    count = 0;
    capacity = 0;
}

void Chunk::write(uint8_t byte, int line) {
    if (capacity < count + 1) {
        auto oldCapacity = capacity;
        capacity = GROW_CAPACITY(oldCapacity);
        code = GROW_ARRAY(uint8_t, code, oldCapacity, capacity);
        lines = GROW_ARRAY(int, lines, oldCapacity, capacity);
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
