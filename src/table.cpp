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

    *entry = {key, value};
    return isNewKey;
}

std::optional<Value> Table::get(ObjString *key) const {
    if (count == 0)
        return std::nullopt;

    auto *entry = findEntry(entries, capacity, key);
    if (!entry->key)
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
    *entry = {nullptr, true};
    return true;
}

ObjString *Table::findString(const char *chars, int length,
                             uint32_t hash) const {
    if (count == 0)
        return nullptr;

    uint32_t index = hash % capacity;
    for (;;) {
        auto *entry = &entries[index];
        if (entry->key == nullptr) {
            // Stop if we find an empty non-tombstone entry
            if (entry->value.isType<Nil>())
                return nullptr;
        } else if (entry->key->size() == length &&
                   entry->key->getHash() == hash &&
                   std::equal(entry->key->data(),
                              entry->key->data() + entry->key->size(), chars)) {
            // Found!
            return entry->key;
        }

        index = (index + 1) % capacity;
    }
}

void Table::adjustCapacity(int newCapacity) {
    auto *newEntries = Allocator::allocate<Entry>(newCapacity);
    for (int i = 0; i < newCapacity; i++) {
        newEntries[i] = {};
    }

    for (int i = 0; i < capacity; i++) {
        auto *entry = &entries[i];
        if (!entry->key)
            continue;

        Entry *dest = findEntry(newEntries, newCapacity, entry->key);
        *dest = *entry;
        count++;
    }

    Allocator::freeArray<Entry>(entries, capacity);
    entries = newEntries;
    capacity = newCapacity;
}

Entry *Table::findEntry(Entry *entries, int capacity, ObjString *key) {
    uint32_t index = key->getHash() % capacity;
    Entry *tombstone = nullptr;
    for (;;) {
        auto *entry = &entries[index];

        if (entry->key == nullptr) {
            if (entry->value.isType<Nil>()) {
                // empty entry
                return tombstone ? tombstone : entry;
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
        auto *entry = &from.entries[i];
        if (entry->key) {
            to.set(entry->key, entry->value);
        }
    }
}

} // namespace Clox
