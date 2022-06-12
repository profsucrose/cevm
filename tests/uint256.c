#include "common.h"

static UInt256 LIMIT = (UInt256){ { 0, 0, 0, ULLONG_MAX } };

void test_UInt256_shiftleft() {
    for (int i = 0; i < 255; i++) {
        UInt256 a = ONE;
        UInt256_shiftleft(&a, i);
        UInt256_print_bits(&a); printf("\n");
    }
}

void test_UInt256_length() {
    assert(UInt256_length(&ZERO) == 0);

    for (int i = 1; i < 255; i++) {
        UInt256 a = ONE;
        UInt256_print_bits(&a); printf("\n");
        UInt256_shiftleft(&a, (uint32_t)i);
        UInt256_print_bits(&a); printf("\n");

        int length = UInt256_length(&a);
        printf("%d == %d\n", length, i + 1);
        assert(length == (i + 1));
    }
}

void test_UInt256_mult() {
    // char u256_out[255];
    // char double_out[255];

    for (int i = 0; i < 255; i++) {
        // uint64_t a = (uint64_t)rand(); // less than 2^31
        // uint64_t b = (uint64_t)rand(); // less than 2^31

        // UInt256 a_u256 = UInt256_from(a);
        // UInt256 b_u256 = UInt256_from(b);

        // double c = (double)a * (double)b * (double)ULLONG_MAX;

        // UInt256_mult(&a_u256, &b_u256);
        // UInt256_mult(&a_u256, &LIMIT);

        // UInt256_print_bits(&a_u256); printf("\n");
        // printf("%llu * %llu * %llu = %.0f\n", a, b, ULLONG_MAX, c);
        // UInt256_print(&a_u256); printf(" = %.0f\n", c);
        
        // UInt256_print_to_buffer(&out);
        // sprintf(double_out, "%f\0", c);
        
    }
}

void test_UInt256_div() {
    char u256_out[255];
    char double_out[255];

    for (int i = 0; i < 255; i++) {
        uint64_t a = (uint64_t)rand(); // less than 2^31
        uint64_t b = (uint64_t)rand(); // less than 2^31

        // UInt256 division
        UInt256 a_u256 = UInt256_from(a);
        UInt256 b_u256 = UInt256_from(b);
        UInt256_add(&a_u256, &LIMIT);
        UInt256_add(&a_u256, &LIMIT);
        // UInt256_div(&a_u256, &b_u256);

        // Native division
        double c = ((double)a + (double)ULLONG_MAX + (double)ULLONG_MAX);

        printf("(%llu + %llu + %llu) = %0.f, u256: ", a, ULLONG_MAX, ULLONG_MAX, c);
        // UInt256_print(&a_u256);
        UInt256_print_bits(&a_u256);
        printf("\n");
    }
}