#ifndef CLOXPP_TABLE_H
#define CLOXPP_TABLE_H

#include <optional>

#include "object.hpp"
#include "value.hpp"

namespace Clox {

struct Entry {
    ObjString *key;
    Value value;

    static Entry makeTombstone();
    bool isTombstone() const;
};

class Table {
public:
    Table();
    ~Table();

    static void addAll(const Table &from, Table &to);

    bool set(ObjString *key, Value value);
    std::optional<Value> get(ObjString *key) const;
    bool deleteKey(ObjString *key);
    ObjString *findString(const char *chars, int length, uint32_t hash) const;

private:
    static constexpr auto TABLE_MAX_LOAD = 0.75;
    static Entry *findEntry(Entry *entries, int capacity, ObjString *key);

    void adjustCapacity(int newCapacity);

    int count;
    int capacity;
    Entry *entries;
};

} // namespace Clox

#endif // !CLOXPP_TABLE_H
