#include "chunk.hpp"
#include "memory.hpp"

namespace Clox {

Chunk::Chunk() : count(0), capacity(0), code(nullptr), constants({}) {}

Chunk::~Chunk() {
    FREE_ARRAY(uint8_t, code, capacity);
    count = 0;
    capacity = 0;
}

void Chunk::write(uint8_t byte) {
    if (capacity < count + 1) {
        auto oldCapacity = capacity;
        capacity = GROW_CAPACITY(oldCapacity);
        code = GROW_ARRAY(uint8_t, code, oldCapacity, capacity);
    }

    code[count] = byte;
    count++;
}

int Chunk::addConstant(Value value) {
    constants.write(value);

    // return the index where the constant was appended
    return constants.count - 1;
}

} // namespace Clox
