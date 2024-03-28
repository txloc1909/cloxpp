#include "object.hpp"
#include "memory.hpp"

namespace Clox {

ObjString::ObjString() : chars(nullptr), length(0) {}

ObjString::ObjString(char *chars, int length)
    : chars(chars), length(length), hash(hashString(chars, length)) {}

ObjString::ObjString(std::string_view str)
    : chars(Allocator::allocate<char>(str.length() + 1)), length(str.length()),
      hash(hashString(str.data(), str.length())) {
    std::copy(str.begin(), str.end(), chars);
    chars[length] = '\0';
}

ObjString::~ObjString() {
    if (chars) {
        Allocator::freeArray<char>(chars, length);
        length = 0;
    }
}

bool ObjString::operator==(const ObjString &other) const {
    return size() == other.size() &&
           std::equal(data(), data() + size(), other.data());
}

const char *ObjString::data() const { return chars; }

int ObjString::size() const { return length; }

uint32_t ObjString::getHash() const { return hash; }

ObjString *ObjString::concatenate(const ObjString &str1,
                                  const ObjString &str2) {
    int length = str1.size() + str2.size();
    char *chars = Allocator::allocate<char>(length + 1);

    auto *mid = std::copy(str1.data(), str1.data() + str1.size(), chars);
    auto *end = std::copy(str2.data(), str2.data() + str2.size(), mid);
    *end = '\0';

    return Allocator::create<ObjString>(chars, length);
}

static uint32_t hashString(const char *key, int length) {
    /* FNV-1a hash function impl */
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= static_cast<uint8_t>(key[i]);
        hash *= 16777619;
    }
    return hash;
}

} // namespace Clox
