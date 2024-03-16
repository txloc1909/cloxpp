#ifndef CLOXPP_COMPILER_H
#define CLOXPP_COMPILER_H

#include <string>

#include "chunk.hpp"

namespace Clox {

bool compile(const std::string &source, Chunk *chunk);

} // namespace Clox

#endif // !CLOXPP_COMPILER_H
