#ifndef CLOXPP_OBJECT_H
#define CLOXPP_OBJECT_H

#include <string_view>

namespace Clox {

class Obj {
public:
    virtual ~Obj() = default;
};

class ObjString : Obj {
public:
    ObjString();
    ObjString(char *chars, int length);
    ObjString(std::string_view str);
    ~ObjString();

    const char *data() const;

private:
    char *chars;
    int length;
};

} // namespace Clox

#endif // !CLOXPP_OBJECT_H
