#include <time.h>
#include <assert.h>

#include "storage.h"
#include "uint256.h"


int main() {
    srand(time(NULL));

    UInt256 limit = UInt256_from(ULLONG_MAX);

    for (int i = 0; i < 1000; i++) {
        uint64_t a = (uint64_t)rand();
        uint64_t b = (uint64_t)rand();

        UInt256 u256_a = UInt256_from(a);
        UInt256 u256_b = UInt256_from(b);

        UInt256_add(&u256_a, &u256_b);
        UInt256_add(&u256_a, &limit);

        printf("Adding %llu %llu %llu\n", a, b, ULLONG_MAX);
        UInt256_print(&u256_a); printf("\n");
        UInt256_print_bits(&u256_a);
        printf("Length: %llu\n", UInt256_length(&u256_a));

        UInt256 ten = UInt256_from(10);
        UInt256_div(&u256_a, &ten);
        printf("Division by 10:\n"); UInt256_print_bits(&u256_a);

        bignum bignum_a, bignum_b, bignum_c;
        int_to_bignum((int)a, &bignum_a);
        int_to_bignum((int)b, &bignum_b);
        add_bignum(&bignum_a, &bignum_b, &bignum_c);

        print_bignum(&bignum_c);
    }

    // for (int i = 0; i < a.lastdigit; i++)
    //     printf("%d", a.digits[i]);
}