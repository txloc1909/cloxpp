#include "object.hpp"
#include "memory.hpp"

namespace Clox {

ObjString::ObjString() : chars(nullptr), length(0) {}

ObjString::ObjString(char *chars, int length) : chars(chars), length(length) {}

ObjString::ObjString(std::string_view str)
    : chars(ALLOCATE(char, str.length() + 1)), length(str.length()) {
    std::copy(str.begin(), str.end(), chars);
    chars[str.length()] = '\0';
}

ObjString::~ObjString() {
    if (chars) {
        FREE_ARRAY(char, chars, length);
        length = 0;
    }
}

bool ObjString::operator==(const ObjString &other) const {
    return size() == other.size() &&
           std::equal(data(), data() + size(), other.data());
}

const char *ObjString::data() const { return chars; }

int ObjString::size() const { return length; }

ObjString ObjString::concatenate(const ObjString &str1, const ObjString &str2) {
    int length = str1.size() + str2.size();
    char *chars = ALLOCATE(char, length + 1);

    auto *mid = std::copy(str1.data(), str1.data() + str1.size(), chars);
    auto *end = std::copy(str2.data(), str2.data() + str2.size(), mid);
    *end = '\0';

    return ObjString(chars, length);
}

} // namespace Clox
