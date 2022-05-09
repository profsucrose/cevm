#include "vendor/bigint/bigint.h"

#include "common.h"

typedef struct {
    UInt256 *key;
    UInt256 *value;
} Entry;

typedef struct {
    Entry **entries;
    size_t capacity;
    size_t length;
} Storage;

void Storage_init(Storage *storage);
void Storage_resize(Storage *storage);
void Storage_insert(Storage *storage, UInt256 *key, UInt256 *value);
UInt256 *Storage_get(Storage *storage, UInt256 *key);