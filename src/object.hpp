#ifndef CLOXPP_OBJECT_H
#define CLOXPP_OBJECT_H

#include <cstdint>
#include <string_view>

namespace Clox {

class Obj {
public:
    template <typename T>
    bool operator==(const T &other) const {
        static_assert(std::is_base_of_v<Obj, T>, "T must be an Obj type");
        return *static_cast<const T *>(this) == other;
    }

    virtual ~Obj() = default;
};

class ObjString : public Obj {
public:
    ObjString();
    ObjString(char *chars, int length);
    ObjString(std::string_view str);
    ~ObjString() override;

    bool operator==(const ObjString &other) const;

    const char *data() const;
    int size() const;
    uint32_t getHash() const;

    static ObjString *concatenate(const ObjString &str1, const ObjString &str2);

private:
    char *chars;
    int length;
    uint32_t hash;
};

static uint32_t hashString(const char *key, int length);

} // namespace Clox

#endif // !CLOXPP_OBJECT_H
