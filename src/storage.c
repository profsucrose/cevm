/**
 * EVM Map Storage implemented as open-addressed
 * hash table using quadratic probing
 */

#include "storage.h"

#define DEFAULT_CAPACITY 10
#define LOAD_FACTOR 0.5
#define GROWTH_RATE 2

static uint64_t cantor(uint64_t k1, uint64_t k2) {
    return (k1 + k2) * (k1 + k2 + 1) / 2;
}

/* Use cantor pairing */
static uint64_t hash(UInt256 *key) {
    return cantor(
        cantor(key->elements[0], key->elements[1]),
        cantor(key->elements[2], key->elements[3])
    );
}

/* Quadratic probing */
static size_t probe(size_t index, size_t probe_index) {
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

void Storage_insert(Storage *storage, const UInt256 *key, const UInt256 *value) {
    size_t index = (size_t)(hash(key) % (uint64_t)storage->capacity),
            probe_index = 0;
    UInt256 *entry_key;

    printf("Hash: %llu; Index: %lu\n", hash(key), index);

    while (storage->entries[index] != NULL) {
        entry_key = storage->entries[index]->key;

        if (UInt256_equals(key, entry_key)) {
            fprintf(stderr, "Key '");
            UInt256_print_to(stderr, key);
            fprintf(stderr, "' already exists in Storage\n");
            exit(1);
        }

        index = probe(index, ++probe_index) % storage->capacity;
    }

    Entry* new_entry = (Entry*)malloc(sizeof(Entry));

    new_entry->key = UInt256_pfrom(0);
    new_entry->value = UInt256_pfrom(0);

    UInt256_copy(key, new_entry->key);
    UInt256_copy(value, new_entry->value);

    storage->entries[index] = new_entry;

    if (++storage->length / storage->capacity >= LOAD_FACTOR)
        Storage_resize(storage);
}

/* Return reference to value that matches given key */
UInt256 *Storage_get(Storage *storage, const UInt256 *key) {
    size_t index = hash(key) % storage->capacity,
            probe_index = 0;

    printf("Index: %lu\n", index);

    while (storage->entries[index] != NULL && 
            storage->entries[index]->key != NULL &&
            !UInt256_equals(key, storage->entries[index]->key)) {
        index = probe(index, ++probe_index) % storage->capacity;
    }
    
    Entry *entry = storage->entries[index];

    // Return 0 if key does not exist
    if (entry == NULL || !UInt256_equals(entry->key, key))
        return &ZERO;

    // Else return retrieved value
    return entry->value;
}