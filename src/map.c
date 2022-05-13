/**
 * EVM Memory/Storage implemented as open-addressed
 * hash tables using quadratic probing
 */

#include "map.h"

#define DEFAULT_CAPACITY 10
#define LOAD_FACTOR 0.5
#define GROWTH_RATE 2

static uint64_t cantor(uint64_t k1, uint64_t k2) {
    return (k1 + k2) / 2 * (k1 + k2 + 1);
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

void VoidMap_init(VoidMap *map) {
    map->capacity = DEFAULT_CAPACITY;
    map->entries = (Entry**)calloc(sizeof(Entry*), map->capacity);
    map->length = 0;
}

void VoidMap_resize(VoidMap *map) {
    size_t old_capacity = map->capacity;
    map->capacity = (int)(((double) old_capacity) * GROWTH_RATE);

    Entry **new_entries = realloc(map->entries, sizeof(Entry) * map->capacity);
    for (int i = 0; i < old_capacity; i++)
        new_entries[i] = map->entries[i];

    map->entries = new_entries;
}

void VoidMap_insert(VoidMap *map, const UInt256 *key, const void *value) {
    size_t index = (size_t)(hash(key) % (uint64_t)map->capacity),
            probe_index = 0;
    UInt256 *entry_key;

    printf("Hash: %llu; Index: %lu\n", hash(key), index);

    while (map->entries[index] != NULL) {
        entry_key = map->entries[index]->key;

        if (UInt256_equals(key, entry_key)) {
            fprintf(stderr, "Key '");
            UInt256_print_to(stderr, key);
            fprintf(stderr, "' already exists in Map\n");
            exit(1);
        }

        index = probe(index, ++probe_index) % map->capacity;
    }

    Entry* new_entry = (Entry*)malloc(sizeof(Entry));

    new_entry->key = UInt256_pfrom(0);
    UInt256_copy(key, new_entry->key);

    new_entry->value = value;

    map->entries[index] = new_entry;

    if (++map->length / map->capacity >= LOAD_FACTOR)
        VoidMap_resize(map);
}

/* Return reference to value that matches given key */
void *VoidMap_get(const VoidMap *map, const UInt256 *key) {
    size_t index = hash(key) % map->capacity,
            probe_index = 0;

    printf("Index: %lu\n", index);

    while (map->entries[index] != NULL && 
            map->entries[index]->key != NULL &&
            !UInt256_equals(key, map->entries[index]->key)) {
        index = probe(index, ++probe_index) % map->capacity;
    }
    
    Entry *entry = map->entries[index];

    if (entry == NULL || !UInt256_equals(entry->key, key)) {
        fprintf(stderr, "Key '");
        UInt256_print_to(stderr, key);
        fprintf(stderr, "' does not exist in VoidMap\n");
        exit(1);
    }

    return entry->value;
}

void IntMap_insert(IntMap *map, const UInt256 *key, const UInt256 *value) {
    VoidMap_insert(map, key, (void*)value);
}

UInt256 *IntMap_get(const IntMap *map, const UInt256 *key) {
    return (UInt256*)VoidMap_get(map, key);
}