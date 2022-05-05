/**
 * EVM Map Storage implemented as open-addressed
 * hash table using quadratic probing
 */

#include "storage.h"

#define DEFAULT_CAPACITY 10
#define LOAD_FACTOR 0.5
#define GROWTH_RATE 2

#define BigInt_equals(a, b) (BigInt_compare(a, b) == 0)

/* Jenkins' One-at-a-Time */
static size_t hash(BigInt *key) {
    char *s = key->digits;
    size_t hash = 0;

    for(; *s; ++s) {
        hash += *s;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

/* Quadratic probing */
static int probe(size_t index, size_t probe_index) {
    return index + probe_index * probe_index;
}

void Storage_init(Storage *storage) {
    storage->capacity = DEFAULT_CAPACITY;
    storage->entries = (Entry**)calloc(sizeof(Entry*), storage->capacity);
    storage->length = 0;
}

void Storage_resize(Storage *storage) {
    size_t old_capacity = storage->capacity;
    storage->capacity = (int)(((double) old_capacity) * GROWTH_RATE);

    Entry **new_entries = realloc(storage->entries, sizeof(Entry) * storage->capacity);
    for (int i = 0; i < old_capacity; i++)
        new_entries[i] = storage->entries[i];

    storage->entries = new_entries;
}

void Storage_insert(Storage *storage, BigInt *key, BigInt *value) {
    size_t index = hash(key) % storage->capacity;

    size_t probe_index = 0;
    while (storage->entries[index] != NULL) {
        BigInt *entry_key = storage->entries[index]->key;

        if (BigInt_equals(key, entry_key)) {
            // TODO: Add better error handling
            sprintf(stderr, "Key '%s' already exists in table", entry_key);
            exit(1);
        }

        index = probe(index, ++probe_index) % storage->capacity;
    }

    Entry* new_entry = (Entry*)malloc(sizeof(Entry));

    new_entry->key = key;
    new_entry->value = value;

    storage->entries[index] = new_entry;

    if (++storage->length / storage->capacity >= LOAD_FACTOR)
        Storage_resize(storage);
}

BigInt *Storage_get(Storage *storage, BigInt *key) {
    size_t index = hash(key) % storage->capacity;

    size_t probe_index = 0;
    while (storage->entries[index] != NULL && 
            !BigInt_equals(key, storage->entries[index]->key)) {
        index = probe(index, ++probe_index) % storage->capacity;
    }

    Entry *entry = storage->entries[index];
    if (entry->key != key) {
        sprintf(stderr, "Key '%s' does not exist in Storage", key->digits);
        exit(1);
    }

    return entry->value;
}