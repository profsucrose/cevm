#include "vendor/bigint/bigint.h"

#include "common.h"

/* Generic HashMap to map
    UInt256 to void* */

typedef struct {
    UInt256 *key;
    void *value;
} Entry;

typedef struct {
    Entry **entries;
    size_t capacity;
    size_t length;
} VoidMap;

void VoidMap_init(VoidMap *storage);
void VoidMap_resize(VoidMap *storage);
void VoidMap_insert(VoidMap *storage, const UInt256 *key, const void *value);
void *VoidMap_get(const VoidMap *storage, const UInt256 *key);

/* IntMap is VoidMap wrapper that casts void* to UInt256* for ease of use */
typedef VoidMap IntMap;

void IntMap_insert(IntMap *map, const IntMap *key, const IntMap *value);
UInt256 *IntMap_get(const IntMap *map, const IntMap *key);