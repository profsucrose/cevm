#include "storage.h"
#include "uint256.h"

int main() {
    #define MULTIPLIER 100000000

    // UInt256 *x = UInt256_from(INT_MAX);

    // for (int i = 0; i < MULTIPLIER; i++)
    //     UInt256_add(x, UInt256_from(INT_MAX));


    BigInt *y = BigInt_construct(INT_MAX);

    for (int i = 0; i < MULTIPLIER; i++)
        BigInt_add(y, BigInt_construct(INT_MAX));

    // BigInt_print(y);

    // __PRINT(x);

    // UInt256_div(&x, UInt256_from(10));

    // printf("%d\n", UInt256_cmp(UInt256_from(0), &x));

    // UInt256_rem(&x, UInt256_from(10));
    // UInt256_print(&x);

    // Storage storage;
    // Storage_init(&storage);

    // Storage_insert(&storage, BigInt_construct(10), BigInt_construct(999));

    // BigInt *result = Storage_get(&storage, BigInt_construct(10));

    // printf("Digits: ");
    // BigInt_print(result);
    // printf("\n");

    // return 0;
}