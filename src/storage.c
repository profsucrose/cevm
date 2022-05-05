/**
 * EVM Map Storage implemented as open-addressed
 * hash table using quadratic probing
 */

#include "storage.h"

#define DEFAULT_CAPACITY 10
#define LOAD_FACTOR 0.5
#define GROWTH_RATE 2

/* djb2 (k=33) hashing algorithm */
static size_t hash(BigInt *key) {
    size_t hash = 5381;

    for (int i = 0; i < key->num_digits; i++)
        hash = ((hash << 5) + hash) + key->digits[i]; /* hash * 33 + c */

    return hash;
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

void Storage_insert(Storage *storage, BigInt *key, BigInt *value) {
    size_t index = hash(key) % storage->capacity;

    size_t probe_index = 0;
    while (storage->entries[index] != NULL) {
        BigInt *entry_key = storage->entries[index]->key;

        if (BigInt_equals(key, entry_key)) {
            // TODO: Add better error handling
            // sprintf(stderr, "Key '%s' already exists in table", entry_key);
            printf("Key '");
            BigInt_print(key);
            printf("' already exists in Storage\n");
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
            storage->entries[index]->key != NULL &&
            !BigInt_equals(key, storage->entries[index]->key)) {
        index = probe(index, ++probe_index) % storage->capacity;
    }
    
    Entry *entry = storage->entries[index];

    if (entry == NULL || !BigInt_equals(entry->key, key)) {
        // sprintf(stderr, "Key '%s' does not exist in Storage", key->digits);
        printf("Key '");
        BigInt_print(key);
        printf("' does not exist in Storage\n");
        exit(1);
    }

    return entry->value;
}