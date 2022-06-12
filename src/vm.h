#ifndef VM_H
#define VM_H

#include "common.h"
#include "storage.h"
#include "memory.h"
#include "logs.h"

typedef enum {
    OP_STOP = 0x00,
    OP_ADD = 0x01,
    OP_MUL = 0x02,
    OP_SUB = 0x03,
    OP_DIV = 0x04,
    OP_SDIV = 0x05,
    OP_MOD = 0x06,
    OP_SMOD = 0x07,
    OP_ADDMOD = 0x08,
    OP_MULMOD = 0x09,
    OP_EXP = 0x0A,
    OP_SIGNEXTEND = 0x0B,
    OP_LT = 0x10,
    OP_GT = 0x11,
    OP_SLT = 0x12,
    OP_SGT = 0x13,
    OP_EQ = 0x14,
    OP_ISZERO = 0x15,
    OP_AND = 0x16,
    OP_OR = 0x17,
    OP_XOR = 0x18,
    OP_NOT = 0x19,
    OP_BYTE = 0x1A,
    OP_SHL = 0x1B,
    OP_SHR = 0x1C,
    OP_SAR = 0x1D,
    OP_SHA3 = 0x20,
    OP_ADDRESS = 0x30,
    OP_BALANCE = 0x31,
    OP_ORIGIN = 0x32,
    OP_CALLER = 0x33,
    OP_CALLVALUE = 0x34,
    OP_CALLDATALOAD = 0x35,
    OP_CALLDATASIZE = 0x36,
    OP_CALLDATACOPY = 0x37,
    OP_CODESIZE = 0x38,
    OP_CODECOPY = 0x39,
    OP_GASPRICE = 0x3A,
    OP_EXTCODESIZE = 0x3B,
    OP_EXTCODECOPY = 0x3C,
    OP_RETURNDATASIZE = 0x3D,
    OP_RETURNDATACOPY = 0x3E,
    OP_EXTCODEHASH = 0x3F,
    OP_BLOCKHASH = 0x40,
    OP_COINBASE = 0x41,
    OP_TIMESTAMP = 0x42,
    OP_NUMBER = 0x43,
    OP_DIFFICULTY = 0x44,
    OP_GASLIMIT = 0x45,
    OP_CHAINID = 0x46,
    OP_SELFBALANCE = 0x47,
    OP_BASEFEE = 0x48,
    OP_POP = 0x50,
    OP_MLOAD = 0x51,
    OP_MSTORE = 0x52,
    OP_MSTORE8 = 0x53,
    OP_SLOAD = 0x54,
    OP_SSTORE = 0x55,
    OP_JUMP = 0x56,
    OP_JUMPI = 0x57,
    OP_PC = 0x58,
    OP_MSIZE = 0x59,
    OP_GAS = 0x5A,
    OP_JUMPDEST = 0x5B,
    OP_PUSH1 = 0x60,
    OP_PUSH2 = 0x61,
    OP_PUSH3 = 0x62,
    OP_PUSH4 = 0x63,
    OP_PUSH5 = 0x64,
    OP_PUSH6 = 0x65,
    OP_PUSH7 = 0x66,
    OP_PUSH8 = 0x67,
    OP_PUSH9 = 0x68,
    OP_PUSH10 = 0x69,
    OP_PUSH11 = 0x6A,
    OP_PUSH12 = 0x6B,
    OP_PUSH13 = 0x6C,
    OP_PUSH14 = 0x6D,
    OP_PUSH15 = 0x6E,
    OP_PUSH16 = 0x6F,
    OP_PUSH17 = 0x70,
    OP_PUSH18 = 0x71,
    OP_PUSH19 = 0x72,
    OP_PUSH20 = 0x73,
    OP_PUSH21 = 0x74,
    OP_PUSH22 = 0x75,
    OP_PUSH23 = 0x76,
    OP_PUSH24 = 0x77,
    OP_PUSH25 = 0x78,
    OP_PUSH26 = 0x79,
    OP_PUSH27 = 0x7A,
    OP_PUSH28 = 0x7B,
    OP_PUSH29 = 0x7C,
    OP_PUSH30 = 0x7D,
    OP_PUSH31 = 0x7E,
    OP_PUSH32 = 0x7F,
    OP_DUP1 = 0x80,
    OP_DUP2 = 0x81,
    OP_DUP3 = 0x82,
    OP_DUP4 = 0x83,
    OP_DUP5 = 0x84,
    OP_DUP6 = 0x85,
    OP_DUP7 = 0x86,
    OP_DUP8 = 0x87,
    OP_DUP9 = 0x88,
    OP_DUP10 = 0x89,
    OP_DUP11 = 0x8A,
    OP_DUP12 = 0x8B,
    OP_DUP13 = 0x8C,
    OP_DUP14 = 0x8D,
    OP_DUP15 = 0x8E,
    OP_DUP16 = 0x8F,
    OP_SWAP1 = 0x90,
    OP_SWAP2 = 0x91,
    OP_SWAP3 = 0x92,
    OP_SWAP4 = 0x93,
    OP_SWAP5 = 0x94,
    OP_SWAP6 = 0x95,
    OP_SWAP7 = 0x96,
    OP_SWAP8 = 0x97,
    OP_SWAP9 = 0x98,
    OP_SWAP10 = 0x99,
    OP_SWAP11 = 0x9A,
    OP_SWAP12 = 0x9B,
    OP_SWAP13 = 0x9C,
    OP_SWAP14 = 0x9D,
    OP_SWAP15 = 0x9E,
    OP_SWAP16 = 0x9F,
    OP_LOG0 = 0xA0,
    OP_LOG1 = 0xA1,
    OP_LOG2 = 0xA2,
    OP_LOG3 = 0xA3,
    OP_LOG4 = 0xA4,
    OP_CREATE = 0xF0,
    OP_CALL = 0xF1,
    OP_CALLCODE = 0xF2,
    OP_RETURN = 0xF3,
    OP_DELEGATECALL = 0xF4,
    OP_CREATE2 = 0xF5,
    OP_STATICCALL = 0xFA,
    OP_REVERT = 0xFD,
    OP_INVALID = 0xFE,
    OP_SELFDESTRUCT = 0xFF
} OpCode;

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