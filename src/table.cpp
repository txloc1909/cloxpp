#include "table.hpp"
#include "memory.hpp"

namespace Clox {

Table::Table() : count(0), capacity(0), entries(nullptr) {}

Table::~Table() {
    if (entries) {
        Allocator::freeArray<Entry>(entries, count);
        count = 0;
        capacity = 0;
    }
}

} // namespace Clox
