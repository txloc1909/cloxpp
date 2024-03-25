#ifndef CLOXPP_MEMORY_H
#define CLOXPP_MEMORY_H

#include <cstddef>

namespace Clox {

#define ALLOCATE(type, count)                                                  \
    (static_cast<type *>(reallocate(NULL, 0, sizeof(type) * (count))))

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount)                          \
    static_cast<type *>(reallocate(pointer, sizeof(type) * (oldCount),         \
                                   sizeof(type) * (newCount)))

#define FREE_ARRAY(type, pointer, oldCount)                                    \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void *reallocate(void *pointer, size_t oldSize, size_t newSize);

} // namespace Clox
#endif // !CLOXPP_MEMORY_H
