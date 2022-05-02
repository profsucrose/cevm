#include "evm.h"
#include <stdlib.h>
#include <stdio.h>

void vm_init(VM *vm, uint8_t *code, int32_t *constants) {
    vm->code = code;
    vm->pc = code;

    vm->stack_top = (int32_t*)vm->stack;

    vm->constants = constants;
}

int32_t vm_eval(VM *vm) {
    #define CONSUME_BYTE() (*vm->pc++)
    #define CONSUME_CONSTANT() (*vm->constants++)
    #define POP() (*--vm->stack_top)
    #define PUSH(constant) ((*vm->stack_top++) = constant)

    uint8_t byte;
    int32_t op1, op2;
    for (;;) {
        switch (byte = CONSUME_BYTE()) {
            case OP_PUSH:
                PUSH(CONSUME_CONSTANT());
                break;
            case OP_ADD:
                PUSH(POP() + POP());
                break;
            case OP_SUB:
                op2 = POP();
                op1 = POP();
                PUSH(op1 - op2);
                break;
            case OP_MUL:
                PUSH(POP() * POP());
                break;
            case OP_DIV:
                op2 = POP();
                op1 = POP();
                PUSH(op1 / op2);
                break;
            case OP_RETURN:
                return POP();
        }
    }

    fprintf(stderr, "Expected OP_RETURN in bytecode\n");
    exit(1);
}