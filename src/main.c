#include <time.h>
#include <assert.h>

#include "storage.h"
#include "uint256.h"

int main() {
    UInt256 *key = UInt256_pfrom(10);
    UInt256 *value = UInt256_pfrom(13751);

    Storage storage;
    Storage_init(&storage);

    printf("Inserting\n");
    Storage_insert(&storage, key, value);
    printf("Finished insert\n");

    UInt256 *result = Storage_get(&storage, UInt256_pfrom(69));

    UInt256_print(result);
}