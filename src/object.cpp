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

const char *ObjString::data() const { return chars; }

} // namespace Clox
