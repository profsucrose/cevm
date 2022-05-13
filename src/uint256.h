#ifndef UINT256_H
#define UINT256_H

#include "common.h"

typedef struct { 
    // Big-endian uint array
    // largest part is at elements[0]
    // smallest is at elements[3]
    uint64_t elements[4]; 
} UInt256;

#define __PRINT(integer) { \
    for (int i = 0; i < 4; i++) \
        printf("%llu ", (integer)->elements[i]); \
}

extern UInt256 ZERO, ONE;

// Init
void UInt256_init(UInt256 *integer, uint64_t value);
UInt256 UInt256_from(uint64_t value);
UInt256 UInt256_from_u256(const UInt256 *integer);
UInt256 *UInt256_pfrom(uint64_t value);

// Comparison
int UInt256_cmp(const UInt256 *a, const UInt256 *b);
bool UInt256_equals(const UInt256 *a, const UInt256 *b);
bool UInt256_gt(const UInt256 *a, const UInt256 *b);
bool UInt256_ge(const UInt256 *a, const UInt256 *b);
bool UInt256_lt(const UInt256 *a, const UInt256 *b);
bool UInt256_le(const UInt256 *a, const UInt256 *b);

// Bitwise ops
void UInt256_and(UInt256 *integer, const UInt256 *op);
void UInt256_or(UInt256 *integer, const UInt256 *op);
void UInt256_xor(UInt256 *integer, const UInt256 *op);
void UInt256_not(UInt256 *integer);
void UInt256_shiftleft(UInt256 *integer, uint32_t op);
void UInt256_shiftright(UInt256 *integer, uint32_t op);

// Arithmetic
void UInt256_add_carry(UInt256 *integer, const UInt256 *op, bool *carry_out);
void UInt256_add(UInt256 *integer, const UInt256 *op);
void UInt256_sub(UInt256 *integer, const UInt256 *op);
void UInt256_mult(UInt256 *integer, const UInt256 *op);
void UInt256_mult_int(UInt256 *integer, uint64_t op);
void UInt256_div(UInt256 *integer, const UInt256 *op);
void UInt256_rem(UInt256 *integer, const UInt256 *op);
void UInt256_pow(UInt256 *integer, const UInt256 *exp);

// Utils
int UInt256_length(const UInt256 *integer);
bool UInt256_get(const UInt256 *integer, uint32_t index);
void UInt256_set(UInt256 *integer, uint32_t index, bool bit);
void __print_bits(size_t const size, void const * const ptr);
void UInt256_copy(const UInt256 *src, UInt256 *dest);
void __UInt256_print_parts(const UInt256 *integer);
void UInt256_print_to(FILE *file, const UInt256 *integer);
void UInt256_print(const UInt256 *integer);
void UInt256_print_bits(const UInt256 *value);

#endif