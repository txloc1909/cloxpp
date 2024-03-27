#ifndef CLOXPP_MEMORY_H
#define CLOXPP_MEMORY_H

#include <cstddef>

namespace Clox {

class Allocator {
public:
    static constexpr size_t growCapacity(size_t capacity) {
        return capacity < 8 ? 8 : (capacity * 2);
    }

    template <typename T>
    static T *allocate(int count) {
        return static_cast<T *>(reallocate(NULL, 0, sizeof(T) * count));
    }

    template <typename T>
    static T *growArray(T *pointer, size_t oldCount, size_t newCount) {
        return static_cast<T *>(
            reallocate(pointer, sizeof(T) * oldCount, sizeof(T) * newCount));
    }

    template <typename T>
    static void freeArray(T *pointer, size_t oldCount) {
        reallocate(pointer, sizeof(T) * oldCount, 0);
    }

    static void *reallocate(void *pointer, size_t oldSize, size_t newSize);
};

} // namespace Clox
#endif // !CLOXPP_MEMORY_H
