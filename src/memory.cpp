#include <cstdlib>

#include "memory.hpp"

namespace Clox {

std::vector<Obj *> Allocator::objects = {};
Table Allocator::strings = {};

void *Allocator::reallocate(void *pointer, size_t /*oldSize*/, size_t newSize) {
    if (newSize == 0) {
        std::free(pointer);
        return nullptr;
    }

    void *result = std::realloc(pointer, newSize);
    if (result == nullptr) {
        exit(1);
    }
    return result;
}

void Allocator::cleanUp() {
    for (auto *obj : objects) {
        delete obj;
    }
    objects.clear();
}

} // namespace Clox
