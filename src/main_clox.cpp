#include "chunk.hpp"

int main() {
    Chunk chunk{};
    chunk.write(OP_RETURN);
    return 0;
}
