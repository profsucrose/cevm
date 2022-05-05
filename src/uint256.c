#include "uint256.h"

UInt256 ZERO = (UInt256){ { 0, 0, 0, 0 } };
UInt256 ONE = (UInt256){ { 0, 0, 0, 1 } };

void UInt256_init(UInt256 *integer, uint64_t value) {
    integer->elements[3] = value; 
    for (int i = 0; i < 3; i++)
        integer->elements[i] = 0;
}

UInt256 *UInt256_from(uint64_t value) {
    UInt256 *new_uint256 = malloc(sizeof(UInt256));

    UInt256_init(new_uint256, value);

    return new_uint256;
}

int UInt256_cmp(const UInt256 *a, const UInt256 *b) {
    for (int i = 0; i < 4; i++) {
        if (a->elements[i] > b->elements[i]) return 1;
        if (a->elements[i] < b->elements[i]) return -1;
    } 

    return 0;
}

bool UInt256_equals(const UInt256 *a, const UInt256 *b) {
    return UInt256_cmp(a, b) == 0;
}

bool UInt256_gt(const UInt256 *a, const UInt256 *b) {
    return UInt256_cmp(a, b) == 1;
}

bool UInt256_lt(const UInt256 *a, const UInt256 *b) {
    return UInt256_cmp(a, b) == -1;
}

void UInt256_and(UInt256 *integer, const UInt256 *op) {
    for (int i = 0; i < 4; i++)
        integer->elements[i] = integer->elements[i] & op->elements[i];
}

void UInt256_or(UInt256 *integer, const UInt256 *op) {
    for (int i = 0; i < 4; i++)
        integer->elements[i] = integer->elements[i] | op->elements[i];
}

void UInt256_xor(UInt256 *integer, const UInt256 *op) {
    for (int i = 0; i < 4; i++)
        integer->elements[i] = integer->elements[i] ^ op->elements[i];
}

void UInt256_not(UInt256 *integer) {
    for (int i = 0; i < 4; i++)
        integer->elements[i] = ~integer->elements[i];
}

/* Returns carry bit (overflow) */
void UInt256_add_carry(UInt256 *integer, const UInt256 *op, bool *carry_out) {
    bool carry = false, new_carry;
    uint64_t a, b;

    for (int i = 3; i >= 0; i--) {
        a = integer->elements[i];
        b = op->elements[i];
    
        new_carry = a > ULLONG_MAX - b;

        integer->elements[i] = a + b + carry;

        carry = new_carry;
    }

    if (carry_out != NULL) 
        *carry_out = carry;
}

void UInt256_add(UInt256 *integer, const UInt256 *op) {
    UInt256_add_carry(integer, op, NULL);
}

void UInt256_sub(UInt256 *integer, const UInt256 *op) {
    UInt256_not(integer);
    UInt256_add(integer, op);
    UInt256_not(integer);
}

void UInt256_mult(UInt256 *integer, const UInt256 *op) {
    while (UInt256_gt(op, &ZERO))
        UInt256_add(integer, op);
}

void UInt256_div(UInt256 *integer, const UInt256 *op) {
    UInt256 counter;
    UInt256_init(&counter, 0);

    UInt256 acc;
    UInt256_copy(op, &acc);

    while (UInt256_lt(&acc, integer)) {
        UInt256_add(&acc, op);
        UInt256_add(&counter, &ONE);
    }

    // Move counter to integer
    UInt256_copy(&counter, integer);
}

void UInt256_rem(UInt256 *integer, const UInt256 *op) {
    UInt256 acc;
    UInt256_copy(op, &acc);

    bool overflow = false;
    while (UInt256_lt(&acc, integer) && !overflow)
        UInt256_add_carry(&acc, op, &overflow);

    UInt256_sub(&acc, op);

    UInt256_sub(integer, &acc);
}

void UInt256_copy(const UInt256 *src, UInt256 *dest) {
    for (int i = 0; i < 4; i++)
        dest->elements[i] = src->elements[i];
}

static UInt256 TEN = (UInt256){ { 0, 0, 0, 10 } };

void UInt256_print(const UInt256 *a) {
    UInt256 tmp;
    UInt256_copy(a, &tmp);

    char stack[76]; // 2^(256-1) < 10^76
    int i = 0;

    UInt256 rem_ten;
    while (UInt256_gt(&tmp, &ZERO)) {
        UInt256_copy(&tmp, &rem_ten);

        UInt256_rem(&rem_ten, &TEN);

        stack[i++] = rem_ten.elements[3];

        UInt256_div(&tmp, &TEN);
    }

    while (i-- > 0)
        printf("%d", stack[i]);
}