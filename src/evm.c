#include "evm.h"

void VM_init(VM *vm, uint8_t *code, int32_t *constants) {
    vm->code = code;
    vm->pc = code;

    vm->stack_top = (int32_t*)vm->stack;

    vm->constants = constants;
}

int32_t VM_eval(VM *vm) {
    #define CONSUME_BYTE() (*vm->pc++)
    #define CONSUME_CONSTANT() (*vm->constants++)
    #define POP() (*--vm->stack_top)
    #define PUSH(a) ((*vm->stack_top++) = a)

    uint8_t byte;
    int32_t op1, op2;
    for (;;) {
        switch (byte = CONSUME_BYTE()) {
            case OP_STOP:
                break;

            case OP_ADD:
                // TODO: Handle `ADD` here
                break;

            case OP_MUL:
                // TODO: Handle `MUL` here
                break;

            case OP_SUB:
                // TODO: Handle `SUB` here
                break;

            case OP_DIV:
                // TODO: Handle `DIV` here
                break;

            case OP_SDIV:
                // TODO: Handle `SDIV` here
                break;

            case OP_MOD:
                // TODO: Handle `MOD` here
                break;

            case OP_SMOD:
                // TODO: Handle `SMOD` here
                break;

            case OP_ADDMOD:
                // TODO: Handle `ADDMOD` here
                break;

            case OP_MULMOD:
                // TODO: Handle `MULMOD` here
                break;

            case OP_EXP:
                // TODO: Handle `EXP` here
                break;

            case OP_SIGNEXTEND:
                // TODO: Handle `SIGNEXTEND` here
                break;

            case OP_LT:
                // TODO: Handle `LT` here
                break;

            case OP_GT:
                // TODO: Handle `GT` here
                break;

            case OP_SLT:
                // TODO: Handle `SLT` here
                break;

            case OP_SGT:
                // TODO: Handle `SGT` here
                break;

            case OP_EQ:
                // TODO: Handle `EQ` here
                break;

            case OP_ISZERO:
                // TODO: Handle `ISZERO` here
                break;

            case OP_AND:
                // TODO: Handle `AND` here
                break;

            case OP_OR:
                // TODO: Handle `OR` here
                break;

            case OP_XOR:
                // TODO: Handle `XOR` here
                break;

            case OP_NOT:
                // TODO: Handle `NOT` here
                break;

            case OP_BYTE:
                // TODO: Handle `BYTE` here
                break;

            case OP_SHL:
                // TODO: Handle `SHL` here
                break;

            case OP_SHR:
                // TODO: Handle `SHR` here
                break;

            case OP_SAR:
                // TODO: Handle `SAR` here
                break;

            case OP_SHA3:
                // TODO: Handle `SHA3` here
                break;

            case OP_ADDRESS:
                // TODO: Handle `ADDRESS` here
                break;

            case OP_BALANCE:
                // TODO: Handle `BALANCE` here
                break;

            case OP_ORIGIN:
                // TODO: Handle `ORIGIN` here
                break;

            case OP_CALLER:
                // TODO: Handle `CALLER` here
                break;

            case OP_CALLVALUE:
                // TODO: Handle `CALLVALUE` here
                break;

            case OP_CALLDATALOAD:
                // TODO: Handle `CALLDATALOAD` here
                break;

            case OP_CALLDATASIZE:
                // TODO: Handle `CALLDATASIZE` here
                break;

            case OP_CALLDATACOPY:
                // TODO: Handle `CALLDATACOPY` here
                break;

            case OP_CODESIZE:
                // TODO: Handle `CODESIZE` here
                break;

            case OP_CODECOPY:
                // TODO: Handle `CODECOPY` here
                break;

            case OP_GASPRICE:
                // TODO: Handle `GASPRICE` here
                break;

            case OP_EXTCODESIZE:
                // TODO: Handle `EXTCODESIZE` here
                break;

            case OP_EXTCODECOPY:
                // TODO: Handle `EXTCODECOPY` here
                break;

            case OP_RETURNDATASIZE:
                // TODO: Handle `RETURNDATASIZE` here
                break;

            case OP_RETURNDATACOPY:
                // TODO: Handle `RETURNDATACOPY` here
                break;

            case OP_EXTCODEHASH:
                // TODO: Handle `EXTCODEHASH` here
                break;

            case OP_BLOCKHASH:
                // TODO: Handle `BLOCKHASH` here
                break;

            case OP_COINBASE:
                // TODO: Handle `COINBASE` here
                break;

            case OP_TIMESTAMP:
                // TODO: Handle `TIMESTAMP` here
                break;

            case OP_NUMBER:
                // TODO: Handle `NUMBER` here
                break;

            case OP_DIFFICULTY:
                // TODO: Handle `DIFFICULTY` here
                break;

            case OP_GASLIMIT:
                // TODO: Handle `GASLIMIT` here
                break;

            case OP_CHAINID:
                // TODO: Handle `CHAINID` here
                break;

            case OP_SELFBALANCE:
                // TODO: Handle `SELFBALANCE` here
                break;

            case OP_BASEFEE:
                // TODO: Handle `BASEFEE` here
                break;

            case OP_POP:
                // TODO: Handle `POP` here
                break;

            case OP_MLOAD:
                // TODO: Handle `MLOAD` here
                break;

            case OP_MSTORE:
                // TODO: Handle `MSTORE` here
                break;

            case OP_MSTORE8:
                // TODO: Handle `MSTORE8` here
                break;

            case OP_SLOAD:
                // TODO: Handle `SLOAD` here
                break;

            case OP_SSTORE:
                // TODO: Handle `SSTORE` here
                break;

            case OP_JUMP:
                // TODO: Handle `JUMP` here
                break;

            case OP_JUMPI:
                // TODO: Handle `JUMPI` here
                break;

            case OP_PC:
                // TODO: Handle `PC` here
                break;

            case OP_MSIZE:
                // TODO: Handle `MSIZE` here
                break;

            case OP_GAS:
                // TODO: Handle `GAS` here
                break;

            case OP_JUMPDEST:
                // TODO: Handle `JUMPDEST` here
                break;

            case OP_PUSH1:
                // TODO: Handle `PUSH1` here
                break;

            case OP_PUSH2:
                // TODO: Handle `PUSH2` here
                break;

            case OP_PUSH3:
                // TODO: Handle `PUSH3` here
                break;

            case OP_PUSH4:
                // TODO: Handle `PUSH4` here
                break;

            case OP_PUSH5:
                // TODO: Handle `PUSH5` here
                break;

            case OP_PUSH6:
                // TODO: Handle `PUSH6` here
                break;

            case OP_PUSH7:
                // TODO: Handle `PUSH7` here
                break;

            case OP_PUSH8:
                // TODO: Handle `PUSH8` here
                break;

            case OP_PUSH9:
                // TODO: Handle `PUSH9` here
                break;

            case OP_PUSH10:
                // TODO: Handle `PUSH10` here
                break;

            case OP_PUSH11:
                // TODO: Handle `PUSH11` here
                break;

            case OP_PUSH12:
                // TODO: Handle `PUSH12` here
                break;

            case OP_PUSH13:
                // TODO: Handle `PUSH13` here
                break;

            case OP_PUSH14:
                // TODO: Handle `PUSH14` here
                break;

            case OP_PUSH15:
                // TODO: Handle `PUSH15` here
                break;

            case OP_PUSH16:
                // TODO: Handle `PUSH16` here
                break;

            case OP_PUSH17:
                // TODO: Handle `PUSH17` here
                break;

            case OP_PUSH18:
                // TODO: Handle `PUSH18` here
                break;

            case OP_PUSH19:
                // TODO: Handle `PUSH19` here
                break;

            case OP_PUSH20:
                // TODO: Handle `PUSH20` here
                break;

            case OP_PUSH21:
                // TODO: Handle `PUSH21` here
                break;

            case OP_PUSH22:
                // TODO: Handle `PUSH22` here
                break;

            case OP_PUSH23:
                // TODO: Handle `PUSH23` here
                break;

            case OP_PUSH24:
                // TODO: Handle `PUSH24` here
                break;

            case OP_PUSH25:
                // TODO: Handle `PUSH25` here
                break;

            case OP_PUSH26:
                // TODO: Handle `PUSH26` here
                break;

            case OP_PUSH27:
                // TODO: Handle `PUSH27` here
                break;

            case OP_PUSH28:
                // TODO: Handle `PUSH28` here
                break;

            case OP_PUSH29:
                // TODO: Handle `PUSH29` here
                break;

            case OP_PUSH30:
                // TODO: Handle `PUSH30` here
                break;

            case OP_PUSH31:
                // TODO: Handle `PUSH31` here
                break;

            case OP_PUSH32:
                // TODO: Handle `PUSH32` here
                break;

            case OP_DUP1:
                // TODO: Handle `DUP1` here
                break;

            case OP_DUP2:
                // TODO: Handle `DUP2` here
                break;

            case OP_DUP3:
                // TODO: Handle `DUP3` here
                break;

            case OP_DUP4:
                // TODO: Handle `DUP4` here
                break;

            case OP_DUP5:
                // TODO: Handle `DUP5` here
                break;

            case OP_DUP6:
                // TODO: Handle `DUP6` here
                break;

            case OP_DUP7:
                // TODO: Handle `DUP7` here
                break;

            case OP_DUP8:
                // TODO: Handle `DUP8` here
                break;

            case OP_DUP9:
                // TODO: Handle `DUP9` here
                break;

            case OP_DUP10:
                // TODO: Handle `DUP10` here
                break;

            case OP_DUP11:
                // TODO: Handle `DUP11` here
                break;

            case OP_DUP12:
                // TODO: Handle `DUP12` here
                break;

            case OP_DUP13:
                // TODO: Handle `DUP13` here
                break;

            case OP_DUP14:
                // TODO: Handle `DUP14` here
                break;

            case OP_DUP15:
                // TODO: Handle `DUP15` here
                break;

            case OP_DUP16:
                // TODO: Handle `DUP16` here
                break;

            case OP_SWAP1:
                // TODO: Handle `SWAP1` here
                break;

            case OP_SWAP2:
                // TODO: Handle `SWAP2` here
                break;

            case OP_SWAP3:
                // TODO: Handle `SWAP3` here
                break;

            case OP_SWAP4:
                // TODO: Handle `SWAP4` here
                break;

            case OP_SWAP5:
                // TODO: Handle `SWAP5` here
                break;

            case OP_SWAP6:
                // TODO: Handle `SWAP6` here
                break;

            case OP_SWAP7:
                // TODO: Handle `SWAP7` here
                break;

            case OP_SWAP8:
                // TODO: Handle `SWAP8` here
                break;

            case OP_SWAP9:
                // TODO: Handle `SWAP9` here
                break;

            case OP_SWAP10:
                // TODO: Handle `SWAP10` here
                break;

            case OP_SWAP11:
                // TODO: Handle `SWAP11` here
                break;

            case OP_SWAP12:
                // TODO: Handle `SWAP12` here
                break;

            case OP_SWAP13:
                // TODO: Handle `SWAP13` here
                break;

            case OP_SWAP14:
                // TODO: Handle `SWAP14` here
                break;

            case OP_SWAP15:
                // TODO: Handle `SWAP15` here
                break;

            case OP_SWAP16:
                // TODO: Handle `SWAP16` here
                break;

            case OP_LOG0:
                // TODO: Handle `LOG0` here
                break;

            case OP_LOG1:
                // TODO: Handle `LOG1` here
                break;

            case OP_LOG2:
                // TODO: Handle `LOG2` here
                break;

            case OP_LOG3:
                // TODO: Handle `LOG3` here
                break;

            case OP_LOG4:
                // TODO: Handle `LOG4` here
                break;

            case OP_CREATE:
                // TODO: Handle `CREATE` here
                break;

            case OP_CALL:
                // TODO: Handle `CALL` here
                break;

            case OP_CALLCODE:
                // TODO: Handle `CALLCODE` here
                break;

            case OP_RETURN:
                // TODO: Handle `RETURN` here
                break;

            case OP_DELEGATECALL:
                // TODO: Handle `DELEGATECALL` here
                break;

            case OP_CREATE2:
                // TODO: Handle `CREATE2` here
                break;

            case OP_STATICCALL:
                // TODO: Handle `STATICCALL` here
                break;

            case OP_REVERT:
                // TODO: Handle `REVERT` here
                break;

            case OP_SELFDESTRUCT:
                // TODO: Handle `SELFDESTRUCT` here
                break;

            default:
                fprintf(stderr, "Unexpected opcode %d\n", byte);
                exit(1);
        }
    }

    fprintf(stderr, "Expected OP_RETURN in bytecode\n");
    exit(1);
}
