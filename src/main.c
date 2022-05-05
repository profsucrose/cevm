#include "storage.h"

int main(void) {
    Storage storage;
    Storage_init(&storage);

    Storage_insert(&storage, BigInt_construct(10), BigInt_construct(38));

    BigInt *result = Storage_get(&storage, BigInt_construct(10));

    printf("%s\n", result->digits);
}