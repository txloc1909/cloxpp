#include "table.hpp"
#include "memory.hpp"

namespace Clox {

Table::Table() : count(0), capacity(0), entries(nullptr) {}

Table::~Table() {
    if (entries) {
        Allocator::freeArray<Entry>(entries, count);
        count = 0;
        capacity = 0;
    }
}

bool Table::set(ObjString *key, Value value) {
    if (count + 1 > capacity * TABLE_MAX_LOAD) {
        adjustCapacity(Allocator::growCapacity(capacity));
    }

    auto *entry = findEntry(entries, capacity, key);
    bool isNewKey = entry->key == nullptr;
    if (isNewKey && entry->value.isType<Nil>())
        count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

std::optional<Value> Table::get(ObjString *key) {
    if (count == 0)
        return std::nullopt;

    auto *entry = findEntry(entries, capacity, key);
    if (entry->key == nullptr)
        return std::nullopt;

    return entry->value;
}

bool Table::deleteKey(ObjString *key) {
    if (count == 0)
        return false;

    auto *entry = findEntry(entries, capacity, key);
    if (entry->key == nullptr)
        return false;

    // Place a tombstone
    entry->key = nullptr;
    entry->value = Nil{};
    return true;
}

void Table::adjustCapacity(int capacity) {
    auto *entries = Allocator::allocate<Entry>(capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i] = Entry{};
    }

    for (int i = 0; i < this->capacity; i++) {
        auto *entry = this->entries + i;
        if (entry->key == nullptr)
            continue;

        Entry *dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        count++;
    }

    Allocator::freeArray<Entry>(this->entries, this->capacity);
    this->entries = entries;
    this->capacity = capacity;
}

Entry *Table::findEntry(Entry *entries, int capacity, ObjString *key) {
    uint32_t index = key->getHash() % capacity;
    Entry *tombstone = nullptr;
    for (;;) {
        auto *entry = entries + index;

        if (entry->key == nullptr) {
            if (entry->value.isType<Nil>()) {
                // empty entry
                return tombstone != nullptr ? tombstone : entry;
            } else {
                // found a tombstone
                if (tombstone == nullptr)
                    tombstone = entry;
            }
        } else if (entry->key == key) {
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

void Table::addAll(const Table &from, Table &to) {
    for (int i = 0; i < from.capacity; i++) {
        auto *entry = from.entries + i;
        if (entry->key == nullptr) {
            to.set(entry->key, entry->value);
        }
    }
}

} // namespace Clox
