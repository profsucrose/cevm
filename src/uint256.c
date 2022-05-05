/*
typedef struct { uint64_t elements[2]; } uint128_t;
// typedef struct { uint128_t elements[2]; } uint256_t;

#define UPPER(x) (x->elements[0])
#define LOWER(x) (x->elements[1])

uint128_t *uint128_from_hex(char* hex) {
    // Assert that `hex` is of form "0x ab cd ef gh ij kl mn op qr st uv wx yz 12 34 56"
}

int uint128_cmp(uint128_t *a, uint128_t *b) {
    // Compare big-ends, if equal
    // then compare small-ends
    return UPPER(a) > UPPER(b) || LOWER(a) > LOWER(b)
        ? 1
        : LOWER(a) == LOWER(b)
            ? 0
            : -1;
}

bool uint128_eq(uint128_t *a, uint128_t *b) {
    return uint128_cmp(a, b) == 0;
}

bool uint128_gt(uint128_t *a, uint128_t *b) {
    return uint128_cmp(a, b) == 1;
}

bool uint128_lt(uint128_t *a, uint128_t *b) {
    return uint128_cmp(a, b) == -1;
}
*/