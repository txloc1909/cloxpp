#ifndef CLOXPP_OBJECT_H
#define CLOXPP_OBJECT_H

#include <cstdint>
#include <functional>
#include <string_view>

#include "chunk.hpp"

namespace Clox {

class Obj {
public:
    virtual ~Obj() = default;
};

class ObjString : public Obj {
public:
    static ObjString *copy(std::string_view str);
    static ObjString *take(char *chars, int length);
    static ObjString *concatenate(const ObjString &str1, const ObjString &str2);

    const char *data() const;
    int size() const;
    uint32_t getHash() const;

private:
    char *chars;
    int length;
    uint32_t hash;

    // constructors are private; ObjString should be created only by
    // `ObjString::copy` and `ObjString::take`
    ObjString(char *chars, int length, uint32_t hash);
    ObjString(std::string_view str, uint32_t hash);
    ~ObjString() override;

    friend class Allocator;
};

enum class FunctionType {
    SCRIPT,
    FUNCTION,
};

class ObjFunction : public Obj {
public:
    ObjFunction();
    ~ObjFunction() override;

    const char *getName() const;
    int getArity() const;
    Chunk *getChunk();

private:
    int arity;
    Chunk chunk;
    ObjString *name;

    friend class Compiler;
};

using NativeFn = std::function<Value(int, Value *)>;

class ObjNative : public Obj {
public:
    ObjNative(NativeFn function);
    ~ObjNative() override;

    const char *getName() const;

    NativeFn function;
};

class ObjClosure : public Obj {
public:
    ObjClosure(ObjFunction *function);
    ~ObjClosure() override;

    ObjFunction *function;
};

} // namespace Clox

#endif // !CLOXPP_OBJECT_H
