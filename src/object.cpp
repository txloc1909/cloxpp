#include "object.hpp"
#include "memory.hpp"

namespace Clox {

static uint32_t hashString(const char *key, int length) {
    /* FNV-1a hash function impl */
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= static_cast<uint8_t>(key[i]);
        hash *= 16777619;
    }
    return hash;
}

ObjString::ObjString(char *chars, int length, uint32_t hash)
    : chars(chars), length(length), hash(hash) {}

ObjString::ObjString(std::string_view str, uint32_t hash)
    : chars(Allocator::allocate<char>(str.length() + 1)), length(str.length()),
      hash(hash) {
    std::copy(str.begin(), str.end(), chars);
    chars[length] = '\0';
}

ObjString::~ObjString() {
    if (chars) {
        Allocator::freeArray<char>(chars, length);
        length = 0;
    }
}

const char *ObjString::data() const { return chars; }

int ObjString::size() const { return length; }

uint32_t ObjString::getHash() const { return hash; }

ObjString *ObjString::copy(std::string_view str) {
    uint32_t hash = hashString(str.data(), str.size());
    ObjString *interned =
        Allocator::getStringSet().findString(str.data(), str.size(), hash);
    if (interned)
        return interned;

    return Allocator::create<ObjString>(str, hash);
}

ObjString *ObjString::take(char *chars, int length) {
    uint32_t hash = hashString(chars, length);
    ObjString *interned =
        Allocator::getStringSet().findString(chars, length, hash);
    if (interned) {
        Allocator::freeArray<char>(chars, length);
        return interned;
    }

    return Allocator::create<ObjString>(chars, length, hash);
}

ObjString *ObjString::concatenate(const ObjString &str1,
                                  const ObjString &str2) {
    int length = str1.size() + str2.size();
    char *chars = Allocator::allocate<char>(length + 1);

    auto *mid = std::copy(str1.data(), str1.data() + str1.size(), chars);
    auto *end = std::copy(str2.data(), str2.data() + str2.size(), mid);
    *end = '\0';

    return Allocator::create<ObjString>(chars, length,
                                        hashString(chars, length));
}

ObjFunction::ObjFunction() : arity(0), chunk({}), name(nullptr) {}

ObjFunction::~ObjFunction() {
    // ObjFunction doesn't own anything, not even its name,
    // so do nothing here
}

Chunk *ObjFunction::getChunk() { return &chunk; }

int ObjFunction::getArity() const { return arity; }

const char *ObjFunction::getName() const {
    return name ? name->data() : "<script>";
}

ObjNative::ObjNative(NativeFn function) : function(function) {}

ObjNative::~ObjNative() {} // do nothing

const char *ObjNative::getName() const { return "<native fn>"; }

} // namespace Clox
