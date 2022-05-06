#include "uint256.h"

UInt256 ZERO = (UInt256){ { 0, 0, 0, 0 } };
UInt256 ONE = (UInt256){ { 0, 0, 0, 1 } };

static bool UInt256_get(const UInt256 *integer, uint32_t index) {
    return integer->elements[index / 64] >> (64 - (index % 64)) & 1;
}

static bool UInt256_set(UInt256 *integer, uint32_t index, bool bit) { 
    uint64_t x = integer->elements[index / 64];

    uint64_t mask = bit << (index % 64);

    integer->elements[3 - index / 64] = (~x & mask) | (x & mask);
}

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

static void UInt256_shiftleft_simple(UInt256 *integer, uint32_t op) {
    // Shift over `op` bits
    for (int i = 255 - op; i >= 0; i--) {
        UInt256_set(integer, i, UInt256_get(integer, i + op));
    }

    // New bits are zeros
    for (int i = 255 - op + 1; i < 256; i++) {
        UInt256_set(integer, i, 0);
    }
}

void UInt256_shiftleft(UInt256 *integer, uint32_t op) {
    int left_word_index, right_word_index;
    uint64_t element, shift_left_side, shift_right_side;

    for (int i = 0; i < 4; i++) {
        // Find words that current word shifts into
        left_word_index = i - 1 - op / 64;
        right_word_index = left_word_index + 1;

        element = integer->elements[i];

        // Isolate bits that go on left word or right word
        // (shift could be split between two new words)
        shift_right_side = element << (op % 64);
        shift_left_side = element >> (64 - op % 64);

        // If doesn't shift into current word, then all new bits are zero
        integer->elements[i] = 0;

        if (left_word_index > 0) integer->elements[left_word_index] |= shift_left_side;
        if (right_word_index > 0) integer->elements[right_word_index] = shift_right_side;
    }
}


void UInt256_shiftright(UInt256 *integer, uint32_t op) {
    int left_word_index, right_word_index;
    uint64_t element, shift_left_side, shift_right_side;

    for (int i = 3; i >= 0; i--) {
        // Find words that current word shifts into
        left_word_index = i + op / 64;
        right_word_index = left_word_index + 1;

        element = integer->elements[i];

        // Isolate bits that go on left word or right word
        // (shift could be split between two new words)
        shift_left_side = element >> (op % 64);
        shift_right_side = element << (64 - (op % 64));

        // If doesn't shift into current word, then all new bits are zero
        integer->elements[i] = 0;

        if (right_word_index < 4) integer->elements[right_word_index] |= shift_right_side;
        if (left_word_index < 4) integer->elements[left_word_index] = shift_left_side;
    }
}

void UInt256_mult(UInt256 *integer, const UInt256 *op) {
    // n = max(size of x, size of y)

    // if (n == 1) return xy;

    // x_l, x_r = leftmost(n/2 of x), rightmost(n/2 of x);
    // y_l, y_r = leftmost(n/2 of y), rightmost(n/2 of y);

    // xy = ((x_l * y_l) << n) + (((x_l * y_r) + (x_r * y_l)) << (n/2)) + x_r * y_r;

    // UInt256 counter;
    // UInt256_copy(op, &counter);

    // while (UInt256_gt(&counter, &ZERO)) {
    //     UInt256_add(integer, op);
    //     UInt256_sub(&counter, &ONE);
    // }
}

void UInt256_mult_int(UInt256 *integer, const uint64_t op) {
    UInt256 big_int;
    UInt256_init(&big_int, op);

    UInt256_mult(integer, &big_int);
}

void UInt256_div(UInt256 *integer, const UInt256 *op) {
    for (int i = 0; i < 4; i++)
        integer->elements[i] /= op->elements[i];

//     UInt256 counter;
//     UInt256_init(&counter, 0);

//     UInt256 acc;
//     UInt256_copy(op, &acc);

//     while (UInt256_lt(&acc, integer)) {
//         UInt256_add(&acc, op);
//         UInt256_add(&counter, &ONE);
//     }

//    // Move counter to integer
//     UInt256_copy(&counter, integer);
}

void UInt256_rem(UInt256 *integer, const UInt256 *op) {
    UInt256 result;
    UInt256_copy(integer, &result);

    UInt256_div(&result, op);

    UInt256_sub(integer, &result);


    // UInt256 acc;
    // UInt256_copy(op, &acc);

    // bool overflow = false;
    // while (UInt256_lt(&acc, integer) && !overflow)
    //     UInt256_add_carry(&acc, op, &overflow);

    // UInt256_sub(&acc, op);

    // UInt256_sub(integer, &acc);
}

void UInt256_copy(const UInt256 *src, UInt256 *dest) {
    for (int i = 0; i < 4; i++)
        dest->elements[i] = src->elements[i];
}

static UInt256 TEN = (UInt256){ { 0, 0, 0, 10 } };

void UInt256_print(const UInt256 *a) {
    UInt256 tmp;
    UInt256_copy(a, &tmp);

    char stack[76]; /* 2^(256-1) < 10^76 */
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