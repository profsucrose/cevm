#include "evm.h"

void VM_init(VM *vm, uint8_t *code, int32_t *constants) {
    vm->code = code;
    vm->pc = 0;

    vm->stack_top = (UInt256*)vm->stack;

    Storage_init(&vm->storage);

    Memory_init(&vm->memory);
}

/* Copy UInt256 */
static UInt256 pop(VM *vm) {
    return *(--vm->stack_top);
}

static UInt256 pop_to_buffer(VM *vm, UInt256 *buffer, uint32_t args) {
    for (int i = args - 1; i >= 0; i--)
        buffer[i] = pop(vm);
}

static void push(VM *vm, const UInt256 value) {
    *vm->stack_top++ = value;
}

static uint8_t consume_byte(VM *vm) {
    return vm->code[vm->pc++];
}

// -2^255 in 2's compliment is 1
static UInt256 MINUS_UINT256_LIMIT = (UInt256){ { 0, 0, 0, 1 } };
static UInt256 MINUS_ONE = (UInt256){ { ULLONG_MAX, ULLONG_MAX, ULLONG_MAX, ULLONG_MAX } };

UInt256 *VM_eval(VM *vm) {
    OpCode opcode;

    UInt256 ops[17];

    #define HANDLE(code, args, block) case code: { \
        pop_to_buffer(vm, &ops, args); \
        block \
        break; \
    }

    for (;;) {
        switch (opcode = consume_opcode(vm)) {
            case OP_STOP: {
                break;
            }

            case OP_ADD: {
                UInt256 b = pop(vm), a = pop(vm);
                UInt256_add(&a, &b);
                push(vm, a);
                break;
            }

            case OP_MUL: {
                UInt256 b = pop(vm), a = pop(vm);
                UInt256_mult(&a, &b);
                push(vm, a);
                break;
            }

            case OP_SUB: {
                UInt256 b = pop(vm), a = pop(vm);
                UInt256_sub(&a, &b);
                push(vm, a);
                break;
            }

            case OP_DIV: {
                UInt256 b = pop(vm), a = pop(vm);
                if (UInt256_equals(&a, &ZERO)) a = ZERO;
                else UInt256_div(&a, &b);
                push(vm, a);
                break;
            }

            case OP_SDIV: {
                UInt256 b = pop(vm), a = pop(vm);

                if (UInt256_equals(&b, &ZERO)) a = ZERO;
                else if (UInt256_equals(&a, &MINUS_UINT256_LIMIT) &&
                        UInt256_equals(&b, &MINUS_ONE))
                    a = MINUS_UINT256_LIMIT;
                else {
                    // Get absolute value of division and store to op1
                    UInt256_div(&a, &b);

                    bool a_negative = UInt256_get(&a, 0);
                    bool b_negative = UInt256_get(&b, 0);

                    if (a_negative + b_negative == 1) {
                        // If one op is negative and other is positive,
                        // then negate division
                        UInt256_compliment(&a);
                    }
                }

                push(vm, a);

                break;
            }

            case OP_MOD: {
                UInt256 b = pop(vm), a = pop(vm);
                
                if (UInt256_equals(&b, &ZERO)) a = ZERO;
                else UInt256_rem(&a, &b);

                push(vm, a);

                break;
            }

            case OP_SMOD: {
                UInt256 b = pop(vm), a = pop(vm);
                
                if (UInt256_equals(&b, &ZERO)) a = ZERO;
                else {
                    UInt256_rem(&a, &b);

                    bool a_negative = UInt256_get(&a, 0);

                    // If a is negative, then negate modulo
                    if (a_negative) UInt256_compliment(&a);
                }

                push(vm, a);

                break;
            }

            case OP_ADDMOD: {
                UInt256 a = pop(vm), b = pop(vm), N = pop(vm);

                if (UInt256_equals(&N, &ZERO)) a = ZERO;
                else {
                    UInt256_add(&a, &b);
                    UInt256_rem(&a, &N);
                }

                push(vm, a);

                break;
            }

            case OP_MULMOD: {
                UInt256 N = pop(vm), b = pop(vm), a = pop(vm);

                if (UInt256_equals(&N, &ZERO)) a = ZERO;
                else {
                    UInt256_mult(&a, &b);
                    UInt256_rem(&a, &N);
                }

                push(vm, a);
            }

            case OP_EXP: {
                UInt256 exponent = pop(vm), a = pop(vm);

                UInt256_pow(&a, &exponent);
                push(vm, a);

                break;
            }
            
            case OP_SIGNEXTEND: {
                UInt256 x = pop(vm), b = pop(vm);

                int t = 256 - 8 * (UInt256_get(&b, 0) + 1);

                for (int i = 0; i < 255; i++)
                    UInt256_set(&b, i, UInt256_get(&x, i >= t ? t : i));

                push(vm, b);

                break;
            }

            case OP_LT: {
                UInt256 b = pop(vm), a = pop(vm);
                push(vm, UInt256_lt(&a, &b) ? ONE : ZERO);
                break;
            }

            case OP_GT: {
                UInt256 b = pop(vm), a = pop(vm);
                push(vm, UInt256_gt(&a, &b) ? ONE : ZERO);
                break;
            }

            case OP_SLT: {
                // Assert ops are in 2's compliment
                UInt256 b = pop(vm), a = pop(vm);

                UInt256 a_abs = a; UInt256_abs(&a_abs);
                UInt256 b_abs = b; UInt256_abs(&b_abs);

                bool abs_lt = UInt256_lt(&a_abs, &b_abs);

                bool a_negative = UInt256_get(&a, 0);
                bool b_negative = UInt256_get(&a, 0);

                // TODO: Shorten logic
                bool lt = (!a_negative && !b_negative && abs_lt) ||    // if a > 0 and b > 0, then true if |a| < |b|
                    (a_negative && !b_negative) ||                     // if a < 0 and b > 0, then true
                    (a_negative && b_negative && !abs_lt);             // if a < 0 and b < 0, then true if |a| > |b|

                push(vm, lt ? ONE : ZERO);

                break;
            }

            case OP_SGT: {
                // Assert ops are in 2's compliment
                UInt256 b = pop(vm), a = pop(vm);

                UInt256 a_abs = a; UInt256_abs(&a_abs);
                UInt256 b_abs = b; UInt256_abs(&b_abs);

                bool abs_gt = UInt256_gt(&a_abs, &b_abs);

                bool a_negative = UInt256_get(&a, 0);
                bool b_negative = UInt256_get(&a, 0);

                bool gt = (!a_negative && !b_negative && abs_gt) ||    // if a > 0 and b > 0, then |a| > |b|
                    (!a_negative && b_negative) ||                     // if a > 0 and b < 0, then true
                    (a_negative && b_negative && !abs_gt);             // if a < 0 and b < 0, then |a| < |b|

                push(vm, gt ? ONE : ZERO);

                break;
            }

            case OP_EQ: {
                UInt256 b = pop(vm), a = pop(vm);

                push(vm, UInt256_equals(&a, &b) ? ONE : ZERO);

                break;
            }

            case OP_ISZERO: {
                UInt256 a = pop(vm);
                push(vm, UInt256_equals(&a, &ZERO) ? ONE : ZERO);
                break;
            }

            case OP_AND: {
                UInt256 b = pop(vm), a = pop(vm);
                UInt256_and(&a, &b);
                push(vm, a);
                break;
            }

            case OP_OR: {
                UInt256 b = pop(vm), a = pop(vm);
                UInt256_or(&a, &b);
                push(vm, a);
                break;
            }

            case OP_XOR: {
                UInt256 b = pop(vm), a = pop(vm);
                UInt256_xor(&a, &b);
                push(vm, a);
                break;
            }

            case OP_NOT: {
                UInt256 a = pop(vm);
                UInt256_not(&a);
                push(vm, a);
                break;
            }

            case OP_BYTE: {
                UInt256 x = pop(vm);
                int i = pop(vm).elements[0];

                // Index starting from most significant byte moving backwards
                // if index is in range
                UInt256 y = i > 31 ? ZERO : UInt256_from((uint64_t)*(((uint8_t*)&x.elements[3]) - i));

                push(vm, y);
                break;
            }

            case OP_SHL: {
                UInt256 value = pop(vm);
                uint32_t shift = (uint32_t)pop(vm).elements[3];
                UInt256_shiftleft(&value, shift);
                push(vm, value);
                break;
            }

            case OP_SHR: {
                UInt256 value = pop(vm);
                uint32_t shift = (uint32_t)pop(vm).elements[3];
                UInt256_shiftright(&value, shift);
                push(vm, value);
                break;
            }

            case OP_SAR: {
                UInt256 value = pop(vm);
                uint32_t shift = (uint32_t)pop(vm).elements[3];

                UInt256_shiftright(&value, shift);

                /* If negative by 2's compliment, then
                shifted bits become 1 instead of 0 */
                if (UInt256_get(&value, 0)) {
                    value.elements[0] |= (ULLONG_MAX << 64 - shift);
                    value.elements[1] |= (ULLONG_MAX << 64 - (shift - 64));
                    value.elements[2] |= (ULLONG_MAX << 64 - (shift - 128));
                    value.elements[3] |= (ULLONG_MAX << 64 - (shift - 192));
                }

                push(vm, value);

                break;
            }

            case OP_SHA3: {
                uint64_t size = pop(vm).elements[3], offset = pop(vm).elements[3];

                SHA3_CTX ctx;
                Keccak_init(&ctx);
                Keccak_update(&ctx, Memory_offset(&vm->memory, offset), size);

                uint64_t buffer[4];
                Keccak_final(&ctx, (uint8_t*)&buffer);

                /* TODO: buffer may have to be reversed?
                (either byte or bit wise) */
                UInt256 hash = (UInt256){ buffer };

                push(vm, hash);
                
                break;
            }

            case OP_ADDRESS: {
                error("Unhandled opcode ADDRESS\n");
                break;
            }

            case OP_BALANCE: {
                error("Unhandled opcode BALANCE\n");
                break;
            }

            case OP_ORIGIN: {
                error("Unhandled opcode ORIGIN\n");
                break;
            }

            case OP_CALLER: {
                error("Unhandled opcode CALLER\n");
                break;
            }

            case OP_CALLVALUE: {
                error("Unhandled opcode CALLVALUE\n");
                break;
            }

            case OP_CALLDATALOAD: {
                error("Unhandled opcode CALLDATALOAD\n");
                break;
            }

            case OP_CALLDATASIZE: {
                error("Unhandled opcode CALLDATASIZE\n");
                break;
            }

            case OP_CALLDATACOPY: {
                error("Unhandled opcode CALLDATACOPY\n");
                break;
            }

            case OP_CODESIZE: {
                error("Unhandled opcode CODESIZE\n");
                break;
            }

            case OP_CODECOPY: {
                error("Unhandled opcode CODECOPY\n");
                break;
            }

            case OP_GASPRICE: {
                error("Unhandled opcode GASPRICE\n");
                break;
            }

            case OP_EXTCODESIZE: {
                error("Unhandled opcode EXTCODESIZE\n");
                break;
            }

            case OP_EXTCODECOPY: {
                error("Unhandled opcode EXTCODECOPY\n");
                break;
            }

            case OP_RETURNDATASIZE: {
                error("Unhandled opcode RETURNDATASIZE\n");
                break;
            }

            case OP_RETURNDATACOPY: {
                error("Unhandled opcode RETURNDATACOPY\n");
                break;
            }

            case OP_EXTCODEHASH: {
                error("Unhandled opcode EXTCODEHASH\n");
                break;
            }

            case OP_BLOCKHASH: {
                error("Unhandled opcode BLOCKHASH\n");
                break;
            }

            case OP_COINBASE: {
                error("Unhandled opcode COINBASE\n");
                break;
            }

            case OP_TIMESTAMP: {
                error("Unhandled opcode TIMESTAMP\n");
                break;
            }

            case OP_NUMBER: {
                error("Unhandled opcode NUMBER\n");
                break;
            }

            case OP_DIFFICULTY: {
                error("Unhandled opcode DIFFICULTY\n");
                break;
            }

            case OP_GASLIMIT: {
                error("Unhandled opcode GASLIMIT\n");
                break;
            }

            case OP_CHAINID: {
                error("Unhandled opcode CHAINID\n");
                break;
            }

            case OP_SELFBALANCE: {
                error("Unhandled opcode SELFBALANCE\n");
                break;
            }

            case OP_BASEFEE: {
                error("Unhandled opcode BASEFEE\n");
                break;
            }

            case OP_POP: {
                pop(vm); // Throw away value
                break;
            }

            /* BM: Byte array operations are little-endian */

            case OP_MLOAD: {
                uint64_t offset = pop(vm).elements[3];
                uint64_t *mem = (uint64_t*)Memory_offset(&vm->memory, offset);
                UInt256 value = (UInt256){ { mem[3], mem[2], mem[1], mem[0] } };
                push(vm, value);
                break;
            }

            case OP_MSTORE: {
                UInt256 value = pop(vm);
                uint64_t offset = pop(vm).elements[3];
                uint64_t *mem = (uint64_t*)Memory_offset(&vm->memory, offset);
                mem[0] = value.elements[3];
                mem[1] = value.elements[2];
                mem[2] = value.elements[1];
                mem[3] = value.elements[0];
                break;
            }

            case OP_MSTORE8: {
                UInt256 value = pop(vm);
                uint64_t offset = pop(vm).elements[3];
                uint64_t *mem = (uint64_t*)Memory_offset(&vm->memory, offset);
                mem[0] = value.elements[3];
                break;
            }

            case OP_SLOAD: {
                UInt256 key = pop(vm), value;
                UInt256_copy(Storage_get(&vm->storage, &key), &value);
                push(vm, value);
                break;
            }

            case OP_SSTORE: {
                UInt256 value = pop(vm), key = pop(vm);
                Storage_insert(&vm->storage, &key, &value);
                break;
            }

            case OP_JUMP: {
                UInt256 counter = pop(vm);
                size_t pc = (size_t)counter.elements[3];
                if (vm->code[pc] == OP_JUMPDEST) vm->pc = (size_t)counter.elements[3];
                else error("Expected JUMP instruction to jump to JUMPDEST, got %llu\n", pc);
                break;
            }

            case OP_JUMPI: {
                UInt256 b = pop(vm), counter = pop(vm);
                if (!UInt256_equals(&b, &ZERO)) vm->pc = counter.elements[3];
                break;
            }

            case OP_PC: {
                push(vm, UInt256_from(vm->pc - 1));
                break;
            }

            case OP_MSIZE: {
                // TODO: Not sure about starting memory capacity/expansion?
                push(vm, UInt256_from(vm->memory.capacity));
                break;
            }

            case OP_GAS: {
                error("Unhandled opcode GAS\n");
                break;
            }

            case OP_JUMPDEST: {
                // Do nothing
                break;
            }

            case OP_PUSH1:
            case OP_PUSH2:
            case OP_PUSH3:
            case OP_PUSH4:
            case OP_PUSH5:
            case OP_PUSH6:
            case OP_PUSH7:
            case OP_PUSH8:
            case OP_PUSH9:
            case OP_PUSH10:
            case OP_PUSH11:
            case OP_PUSH12:
            case OP_PUSH13:
            case OP_PUSH14:
            case OP_PUSH15:
            case OP_PUSH16:
            case OP_PUSH17:
            case OP_PUSH18:
            case OP_PUSH19:
            case OP_PUSH20:
            case OP_PUSH21:
            case OP_PUSH22:
            case OP_PUSH23:
            case OP_PUSH24:
            case OP_PUSH25:
            case OP_PUSH26:
            case OP_PUSH27:
            case OP_PUSH28:
            case OP_PUSH29:
            case OP_PUSH30:
            case OP_PUSH31:
            case OP_PUSH32: {
                // TODO: Handle `PUSH32` here
                uint64_t length = opcode - OP_PUSH1 + 1;

                UInt256 value = ZERO;
                uint8_t *buffer = &value.elements[0] + 7; // Last byte in smallest word

                for (int i = 0; i < length; i++)
                    *(buffer - i) = consume_byte(vm);
                
                push(vm, value);
                
                break;
            }

            case OP_DUP1:
            case OP_DUP2:
            case OP_DUP3:
            case OP_DUP4:
            case OP_DUP5:
            case OP_DUP6:
            case OP_DUP7:
            case OP_DUP8:
            case OP_DUP9:
            case OP_DUP10:
            case OP_DUP11:
            case OP_DUP12:
            case OP_DUP13:
            case OP_DUP14:
            case OP_DUP15:
            case OP_DUP16: {
                uint64_t stack_offset = opcode - OP_DUP1;
                for (int i = stack_offset + 1; i > 0; i--)
                    vm->stack[i] = vm->stack[i - 1];
                vm->stack[0] = vm->stack[stack_offset + 1];
                break;
            }

            case OP_SWAP1:
            case OP_SWAP2:
            case OP_SWAP3:
            case OP_SWAP4:
            case OP_SWAP5:
            case OP_SWAP6:
            case OP_SWAP7:
            case OP_SWAP8:
            case OP_SWAP9:
            case OP_SWAP10:
            case OP_SWAP11:
            case OP_SWAP12:
            case OP_SWAP13:
            case OP_SWAP14:
            case OP_SWAP15:
            case OP_SWAP16: {
                uint64_t swap_index = opcode - OP_SWAP1 + 1;
                UInt256 tmp = vm->stack[0];
                vm->stack[0] = vm->stack[swap_index];
                vm->stack[swap_index] = tmp;
                break;
            }

            case OP_LOG0:
            case OP_LOG1:
            case OP_LOG2:
            case OP_LOG3:
            case OP_LOG4: {
                error("Unhandled opcode LOG%d\n", (int)(opcode - OP_LOG0));
                break;
            }

            case OP_CREATE: {
                error("Unhandled opcode CREATE\n");
                break;
            }

            case OP_CALL: {
                UInt256 ret_size = pop(vm), ret_offset = pop(vm), args_size = pop(vm),
                    args_offset = pop(vm), value = pop(vm), address = pop(vm), gas = pop(vm);

                // TODO: Handle hardcoded CALL addresses for Playdate utils
                error("Unhandled opcode CALL\n");

                break;
            }

            case OP_CALLCODE: {
                UInt256 ret_size = pop(vm), ret_offset = pop(vm), args_size = pop(vm),
                    args_offset = pop(vm), value = pop(vm), address = pop(vm), gas = pop(vm);

                // TODO: Should maybe handle this?
                error("Unhandled opcode CALLCODE\n");

                break;
            }

            case OP_RETURN: {
                // TODO: Should also maybe handle this?
                error("Unhandled opcode RETURN\n");
                break;
            }

            case OP_DELEGATECALL: {
                // TODO: Should also maybe handle this?
                error("Unhandled opcode DELEGATECALL\n");
                break;
            }

            case OP_CREATE2: {
                error("Unhandled opcode CREATE2\n");
                break;
            }

            case OP_STATICCALL: {
                // TODO: Maybe handle this as well for Playdate calls?
                error("Unhandled opcode STATICCALL\n");
                break;
            }

            case OP_REVERT: {
                error("Unhandled opcode REVERT\n");
                break;
            }

            case OP_SELFDESTRUCT: {
                error("Unhandled opcode SELFDESTRUCT\n");
                break;
            }

            default: {
                error("Unexpected opcode %d\n", opcode);
            }
        }
    }

    error("Expected RETURN in bytecode\n");
}
