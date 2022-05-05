#ifndef uint256_h_
#define uint256_h_

#include "common.h"

typedef struct { 
    // Big-endian uint array
    // largest part is at elements[0]
    // smallest is at elements[3]
    uint64_t elements[4]; 
} UInt256;

#define ADD_CONSTANT(integer, op) { \
    UInt256 *tmp = UInt256_from(op); \
    UInt256_add(integer, tmp); \
    free(tmp); \
}

extern UInt256 ZERO, ONE;

// Init
void UInt256_init(UInt256 *integer, uint64_t value);
UInt256 *UInt256_from(uint64_t value);

// Comparison
int UInt256_cmp(const UInt256 *a, const UInt256 *b);
bool UInt256_equals(const UInt256 *a, const UInt256 *b);
bool UInt256_gt(const UInt256 *a, const UInt256 *b);
bool UInt256_lt(const UInt256 *a, const UInt256 *b);

// Bitwise ops
void UInt256_and(UInt256 *integer, const UInt256 *op);
void UInt256_or(UInt256 *integer, const UInt256 *op);
void UInt256_xor(UInt256 *integer, const UInt256 *op);
void UInt256_not(UInt256 *integer);
void UInt256_add_carry(UInt256 *integer, const UInt256 *op, bool *carry_out);
void UInt256_add(UInt256 *integer, const UInt256 *op);

// Arithmetic
void UInt256_sub(UInt256 *integer, const UInt256 *op);
void UInt256_mult(UInt256 *integer, const UInt256 *op);
void UInt256_div(UInt256 *integer, const UInt256 *op);
void UInt256_rem(UInt256 *integer, const UInt256 *op);

// Utils
void UInt256_copy(const UInt256 *src, UInt256 *dest);
void UInt256_print(const UInt256 *a);

#endif