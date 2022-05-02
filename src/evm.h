#ifndef evm_h
#define evm_h

#include <stdint.h>

typedef enum {
    OP_PUSH,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_RETURN
} OpCode;

#define STACK_MAX 256

typedef struct {
    // Bytecode + program counter
    uint8_t *code;
    uint8_t *pc;

    // Stack for manipulating data
    int32_t stack[STACK_MAX];
    int32_t *stack_top;

    // Separate bytecode/constants for memory efficiency
    // no counter needed since iterated through linearly (no JUMPs)
    int32_t *constants;
} VM;

void vm_init(VM *vm, uint8_t *code, int32_t *constants);
int32_t vm_eval(VM *vm);

#endif