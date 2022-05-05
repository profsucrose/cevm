#include "vendor/bigint/bigint.h"

#include "common.h"

typedef struct {
    BigInt *key;
    BigInt *value;
} Entry;

typedef struct {
    Entry **entries;
    size_t capacity;
    size_t length;
} Storage;

void Storage_init(Storage *storage);
void Storage_resize(Storage *storage);
void Storage_insert(Storage *storage, BigInt *key, BigInt *value);
BigInt *Storage_get(Storage *storage, BigInt *key);