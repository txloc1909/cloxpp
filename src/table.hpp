#ifndef CLOXPP_TABLE_H
#define CLOXPP_TABLE_H

#include "object.hpp"
#include "value.hpp"

namespace Clox {

struct Entry {
    ObjString *key;
    Value value;
};

class Table {
public:
    Table();
    ~Table();

private:
    int count;
    int capacity;
    Entry *entries;
};

} // namespace Clox

#endif // !CLOXPP_TABLE_H
