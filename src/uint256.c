#include "uint256.h"

UInt256 ZERO = (UInt256){ { 0, 0, 0, 0 } };
UInt256 ONE = (UInt256){ { 0, 0, 0, 1 } };
UInt256 TWO = (UInt256){ { 0, 0, 0, 2 } };

bool UInt256_get(const UInt256 *integer, uint32_t index) {
    return (integer->elements[index / 64] >> (63 - (index % 64))) & 1;
}

void UInt256_set(UInt256 *integer, uint32_t index, bool bit) { 
    uint64_t x = integer->elements[index / 64];

    uint64_t mask = 1 << (64 - (index % 64));

    integer->elements[index / 64] = bit
        ? x | mask
        : x & ~mask;
}

UInt256 UInt256_from(uint64_t value) {
    return (UInt256){ { 0, 0, 0, value } };
}

UInt256 UInt256_from_u256(const UInt256 *integer) {
    return (UInt256){ { integer->elements[0], integer->elements[1], integer->elements[2], integer->elements[3] } };
}

void UInt256_init(UInt256 *integer, uint64_t value) {
    integer->elements[0] = 0;
    integer->elements[1] = 0;
    integer->elements[2] = 0;
    integer->elements[3] = value;
}

UInt256 *UInt256_pfrom(uint64_t value) {
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

bool UInt256_ge(const UInt256 *a, const UInt256 *b) {
    int cmp = UInt256_cmp(a, b);
    return cmp == 1 || cmp == 0;
}

bool UInt256_lt(const UInt256 *a, const UInt256 *b) {
    return UInt256_cmp(a, b) == -1;
}

bool UInt256_le(const UInt256 *a, const UInt256 *b) {
    int cmp = UInt256_cmp(a, b);
    return cmp == -1 || cmp == 0;
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
    /* Add 2's complement */
    UInt256_not(integer);
    UInt256_add(integer, op);
    UInt256_not(integer);
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
        shift_left_side = element >> (64 - (op % 64));
        shift_right_side = element << (op % 64);

        // If doesn't shift into current word, then all new bits are zero
        integer->elements[i] = 0;

        if (right_word_index >= 0) integer->elements[right_word_index] = shift_right_side;
        if (left_word_index >= 0) integer->elements[left_word_index] |= shift_left_side;
    }
}

void UInt256_shiftright(UInt256 *integer, uint32_t op) {
    int left_word_index, right_word_index;
    uint64_t element, shift_left_side, shift_right_side;

    for (int i = 3; i >= 0; i--) {
        // Find words that current word shifts into
        right_word_index = i + 1 + op / 64;
        left_word_index = right_word_index - 1;

        element = integer->elements[i];

        // Isolate bits that go on left word or right word
        // (shift could be split between two new words)
        shift_left_side = element >> (op % 64);
        shift_right_side = (element << (64 - op % 64)) >> (op % 64);

        // If doesn't shift into current word, then all new bits are zero
        integer->elements[i] = 0;

        if (left_word_index < 4) integer->elements[left_word_index] = shift_left_side;
        if (right_word_index < 4) integer->elements[right_word_index] |= shift_right_side;
    }
}

/* Get length of UInt256 (number of digits after MSB) */
int UInt256_length(const UInt256 *integer) {
    uint32_t *ptr = (uint32_t*)&integer->elements[0], lz, word;
    ptr--; // Move back 1 for left-most uint32

    for (int i = 7; i >= 0; i--) {
        word = *(ptr + i);
        if (word != 0) {
            lz = __builtin_clz(word);
            return (32 - lz) + 32 * (7 - i);
        }
    }

    return 0;
}

void UInt256_div(UInt256 *integer, const UInt256 *op) {
    if (UInt256_equals(op, &ZERO)) {
        fprintf(stderr, "Tried to divide UInt256 by zero\n");
        exit(1);
    }

    /* Binary long division (TODO: Karatsuba) */

    UInt256 result = UInt256_from(0), 
            dividend = UInt256_from(0);

    // Start at MSB of `integer` and move right
    for (int i = 256 - UInt256_length(integer); i <= 255; i++) {
        UInt256_shiftleft(&dividend, 1);
        
        if (UInt256_get(integer, i))
            UInt256_add(&dividend, &ONE);

        if (UInt256_ge(&dividend, op)) {
            UInt256_sub(&dividend, op);
            UInt256_add(&result, &ONE);
        }

        if (i < 255)
            UInt256_shiftleft(&result, 1);
    }

    UInt256_copy(&result, integer);
}

void UInt256_mult(UInt256 *integer, const UInt256 *op) {
    /* Long multiplication (TODO: Karatsuba) */

    UInt256 result = UInt256_from(0);

    UInt256 tmp_integer = UInt256_from_u256(integer);

    for (int i = 255; i >= 256 - UInt256_length(op); i--) {
        // TODO: only shift on add
        if (UInt256_get(op, i))
            UInt256_add(&result, &tmp_integer);

        UInt256_shiftleft(&tmp_integer, 1);
    }

    // Move result to input
    UInt256_copy(&result, integer);
}

void UInt256_mult_int(UInt256 *integer, const uint64_t op) {
    UInt256 big_int = UInt256_from(op);
    UInt256_mult(integer, &big_int);
}

void UInt256_rem(UInt256 *integer, const UInt256 *op) {
    UInt256 result = UInt256_from_u256(integer);
    UInt256_div(&result, op);
    UInt256_mult(&result, op);
    UInt256_sub(integer, &result);
}

void UInt256_pow(UInt256 *integer, const UInt256 *exp) {
    if (UInt256_equals(exp, &ZERO)) {
        UInt256_copy(&ONE, integer);
        return;
    }

    // If odd, store flag and make even
    bool is_odd = exp->elements[3] & 1;

    UInt256 result = UInt256_from_u256(integer);

    // Square result for half exponent
    UInt256 half_exp = UInt256_from_u256(exp);
    if (is_odd) UInt256_sub(&half_exp, &ONE);

    UInt256_div(&half_exp, &TWO);
    UInt256_pow(&result, &half_exp);
    UInt256_mult(&result, &result);

    // If odd then add result
    if (is_odd) UInt256_mult(&result, integer);

    UInt256_copy(&result, integer);
}

void UInt256_copy(const UInt256 *src, UInt256 *dest) {
    for (int i = 0; i < 4; i++)
        dest->elements[i] = src->elements[i];
}

static UInt256 TEN = (UInt256){ { 0, 0, 0, 10 } };

void __print_bits(size_t size, const void *ptr) {
    uint8_t *b = (uint8_t*)ptr;
    uint8_t byte;
    
    for (int i = size - 1; i >= 0; i--) {
        for (int j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
}

void UInt256_print_bits(const UInt256 *value) {
    for (int i = 0; i < 4; i++)
        __print_bits(sizeof(uint64_t), (void*)&value->elements[i]);

    printf("\n");
}

void UInt256_print(const UInt256 *integer) {
    UInt256 tmp = UInt256_from_u256(integer);

    char stack[76]; /* 2^(256-1) < 10^76 */
    int i = 0;

    UInt256 rem_ten;
    while (UInt256_gt(&tmp, &ZERO)) {
        UInt256_copy(&tmp, &rem_ten);

        UInt256_rem(&rem_ten, &TEN);

        stack[i++] = 48 + rem_ten.elements[3];

        UInt256_div(&tmp, &TEN);
    }

    while (i-- > 0)
        putchar(stack[i]);
}