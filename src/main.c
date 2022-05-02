#include "evm.h"

#include <stdio.h>

int main(void) {
    uint8_t instructions[] = (uint8_t[]) {
        OP_PUSH, // PUSH 5
        OP_PUSH, // PUSH 3
        OP_ADD,

        OP_PUSH, // PUSH 3
        OP_ADD,

        OP_PUSH, // PUSH 3
        OP_MUL,

        OP_PUSH, // PUSH 3 
        OP_DIV,
        
        OP_RETURN
    };

    int32_t constants[] = (int32_t[]) {
        5, 3, 3, 3, 3,
    };

    VM vm;
    vm_init(&vm, &instructions[0], &constants[0]);

    int result = vm_eval(&vm);

    printf("Result: %d\n", result);
}