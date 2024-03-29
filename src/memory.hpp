#ifndef CLOXPP_MEMORY_H
#define CLOXPP_MEMORY_H

#include <cstddef>
#include <type_traits>
#include <vector>

#include "object.hpp"
#include "table.hpp"

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

    template <typename T, typename... Args>
    static T *create(Args... args) {
        static_assert(std::is_base_of<Obj, T>::value, "T must be an Obj type");
        T *newT = new T(std::forward<Args>(args)...);
        objects.push_back(newT);

        if constexpr (std::is_same<T, ObjString>::value) {
            // intern the created string
            strings.set(static_cast<ObjString *>(newT), Nil{});
        }

        return newT;
    }

    static const Table &getStringSet() { return strings; }

    static void *reallocate(void *pointer, size_t oldSize, size_t newSize);

    static void cleanUp();

private:
    static std::vector<Obj *> objects;
    static Table strings; // string interning

    Allocator(){};
    ~Allocator(){};
};

} // namespace Clox
#endif // !CLOXPP_MEMORY_H
