#ifndef VM_H
#define VM_H

#include "common.h"
#include "storage.h"
#include "memory.h"
#include "logs.h"
#include "ops.h"

/* 
 * For simplicity, store Stack, Contracts, Calldata,
 * and Return Buffer as static arrays
 */
#define STACK_MAX 1024
#define CONTRACT_MAX 1024
#define CALLDATA_MAX 1024
#define RET_MAX 1024

typedef struct {
    uint8_t *code;
    size_t code_size;

    size_t address;

    Storage storage;
} Contract;

typedef struct {
    uint8_t *code;
    size_t code_size;

    UInt256 value;

    size_t address;

    /* Index of calling Contract in Contract array */
    size_t sender;

    UInt256 stack[STACK_MAX];
    UInt256 *stack_top;

    Memory *memory;

    Storage *storage;

    uint8_t *calldata;
    size_t calldata_size;

    uint8_t *return_data;
    size_t return_data_size;
} Context;

typedef struct {
    /*
     * For purposes of simplified EVM, (only need to execute one root contract
     * at a time) represent contracts as index-addressable stack
     */
    Contract *contracts[CONTRACT_MAX];
    size_t contracts_length;
} VM;

void VM_init(VM *vm);
bool VM_call(VM *vm, Context *ctx, Logs *out_logs);

#endif