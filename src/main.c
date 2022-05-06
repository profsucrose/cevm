#include "storage.h"
#include "uint256.h"

// Assumes little endian
void print_bits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

int main() {
    UInt256 *value = UInt256_from(ULLONG_MAX);
    UInt256_shiftleft(value, 65);

    // print_bits(sizeof(uint64_t), &limit);
    __PRINT(value);

    // for (int i = 0; i < MULTIPLIER; i++)
    //     UInt256_add(x, UInt256_from(INT_MAX));


    // BigInt *y = BigInt_construct(INT_MAX);

    // for (int i = 0; i < MULTIPLIER; i++)
    //     BigInt_add(y, BigInt_construct(INT_MAX));

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