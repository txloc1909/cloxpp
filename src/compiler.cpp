#include "compiler.hpp"

namespace Clox {

bool compile(const std::string & /*source*/, Chunk *chunk) {
    auto constant = chunk->addConstant(1.0);
    chunk->write(OP_CONSTANT, 1);
    chunk->write(constant, 1);
    chunk->write(OP_RETURN, 1);

    return true;
}

} // namespace Clox
