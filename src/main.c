#include "storage.h"

int main(void) {
    Storage storage;
    Storage_init(&storage);

    Storage_insert(&storage, BigInt_construct(10), BigInt_construct(999));

    BigInt *result = Storage_get(&storage, BigInt_construct(10));

    printf("Digits: ");
    BigInt_print(result);
    printf("\n");

    return 0;
}