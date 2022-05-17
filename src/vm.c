#include "vm.h"

void VM_init(VM *vm) {
    // Contracts
    vm->contracts_length = 0;

    // Storage
    Storage_init(&vm->storage);

    // Memory
    Memory_init(&vm->memory);
}

static size_t add_contract(VM *vm, const VM *contract) {
    vm->contracts[vm->contracts_length++] = contract;
    return vm->contracts_length - 1;
}

// -2^255 in 2's compliment is 1
static UInt256 MINUS_UINT256_LIMIT = (UInt256){ { 0, 0, 0, 1 } };
static UInt256 MINUS_ONE = (UInt256){ { ULLONG_MAX, ULLONG_MAX, ULLONG_MAX, ULLONG_MAX } };

void *VM_call(VM *vm, const Contract *contract, const size_t caller_address, const uint8_t *calldata, const size_t calldata_size, uint8_t **out_return_buffer, size_t *out_return_buffer_size) {
    UInt256 stack[STACK_MAX];
    UInt256 *stack_top = stack;

    size_t pc = 0;

    #define CONSUME_BYTE() (contract->code[pc++])

    // Copy UInt256 for stack operations
    #define POP() (*--stack_top)
    #define PUSH(value) *stack_top++ = value

    OpCode opcode;

    for (;;) {
        switch (opcode = CONSUME_BYTE()) {
            case OP_STOP: {
                break;
            }

            case OP_ADD: {
                UInt256 b = POP(), a = POP();
                UInt256_add(&a, &b);
                PUSH(a);
                break;
            }

            case OP_MUL: {
                UInt256 b = POP(), a = POP();
                UInt256_mult(&a, &b);
                PUSH(a);
                break;
            }

            case OP_SUB: {
                UInt256 b = POP(), a = POP();
                UInt256_sub(&a, &b);
                PUSH(a);
                break;
            }

            case OP_DIV: {
                UInt256 b = POP(), a = POP();
                if (UInt256_equals(&a, &ZERO)) a = ZERO;
                else UInt256_div(&a, &b);
                PUSH(a);
                break;
            }

            case OP_SDIV: {
                UInt256 b = POP(), a = POP();

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

                PUSH(a);

                break;
            }

            case OP_MOD: {
                UInt256 b = POP(), a = POP();
                
                if (UInt256_equals(&b, &ZERO)) a = ZERO;
                else UInt256_rem(&a, &b);

                PUSH(a);

                break;
            }

            case OP_SMOD: {
                UInt256 b = POP(), a = POP();
                
                if (UInt256_equals(&b, &ZERO)) a = ZERO;
                else {
                    UInt256_rem(&a, &b);

                    bool a_negative = UInt256_get(&a, 0);

                    // If a is negative, then negate modulo
                    if (a_negative) UInt256_compliment(&a);
                }

                PUSH(a);

                break;
            }

            case OP_ADDMOD: {
                UInt256 a = POP(), b = POP(), N = POP();

                if (UInt256_equals(&N, &ZERO)) a = ZERO;
                else {
                    UInt256_add(&a, &b);
                    UInt256_rem(&a, &N);
                }

                PUSH(a);

                break;
            }

            case OP_MULMOD: {
                UInt256 N = POP(), b = POP(), a = POP();

                if (UInt256_equals(&N, &ZERO)) a = ZERO;
                else {
                    UInt256_mult(&a, &b);
                    UInt256_rem(&a, &N);
                }

                PUSH(a);
            }

            case OP_EXP: {
                UInt256 exponent = POP(), a = POP();

                UInt256_pow(&a, &exponent);
                PUSH(a);

                break;
            }
            
            case OP_SIGNEXTEND: {
                UInt256 x = POP(), b = POP();

                int t = 256 - 8 * (UInt256_get(&b, 0) + 1);

                for (int i = 0; i < 255; i++)
                    UInt256_set(&b, i, UInt256_get(&x, i >= t ? t : i));

                PUSH(b);

                break;
            }

            case OP_LT: {
                UInt256 b = POP(), a = POP();
                PUSH(UInt256_lt(&a, &b) ? ONE : ZERO);
                break;
            }

            case OP_GT: {
                UInt256 b = POP(), a = POP();
                PUSH(UInt256_gt(&a, &b) ? ONE : ZERO);
                break;
            }

            case OP_SLT: {
                // Assert ops are in 2's compliment
                UInt256 b = POP(), a = POP();

                UInt256 a_abs = a; UInt256_abs(&a_abs);
                UInt256 b_abs = b; UInt256_abs(&b_abs);

                bool abs_lt = UInt256_lt(&a_abs, &b_abs);

                bool a_negative = UInt256_get(&a, 0);
                bool b_negative = UInt256_get(&a, 0);

                // TODO: Shorten logic
                bool lt = (!a_negative && !b_negative && abs_lt) ||    // if a > 0 and b > 0, then true if |a| < |b|
                    (a_negative && !b_negative) ||                     // if a < 0 and b > 0, then true
                    (a_negative && b_negative && !abs_lt);             // if a < 0 and b < 0, then true if |a| > |b|

                PUSH(lt ? ONE : ZERO);

                break;
            }

            case OP_SGT: {
                // Assert ops are in 2's compliment
                UInt256 b = POP(), a = POP();

                UInt256 a_abs = a; UInt256_abs(&a_abs);
                UInt256 b_abs = b; UInt256_abs(&b_abs);

                bool abs_gt = UInt256_gt(&a_abs, &b_abs);

                bool a_negative = UInt256_get(&a, 0);
                bool b_negative = UInt256_get(&a, 0);

                bool gt = (!a_negative && !b_negative && abs_gt) ||    // if a > 0 and b > 0, then |a| > |b|
                    (!a_negative && b_negative) ||                     // if a > 0 and b < 0, then true
                    (a_negative && b_negative && !abs_gt);             // if a < 0 and b < 0, then |a| < |b|

                PUSH(gt ? ONE : ZERO);

                break;
            }

            case OP_EQ: {
                UInt256 b = POP(), a = POP();

                PUSH(UInt256_equals(&a, &b) ? ONE : ZERO);

                break;
            }

            case OP_ISZERO: {
                UInt256 a = POP();
                PUSH(UInt256_equals(&a, &ZERO) ? ONE : ZERO);
                break;
            }

            case OP_AND: {
                UInt256 b = POP(), a = POP();
                UInt256_and(&a, &b);
                PUSH(a);
                break;
            }

            case OP_OR: {
                UInt256 b = POP(), a = POP();
                UInt256_or(&a, &b);
                PUSH(a);
                break;
            }

            case OP_XOR: {
                UInt256 b = POP(), a = POP();
                UInt256_xor(&a, &b);
                PUSH(a);
                break;
            }

            case OP_NOT: {
                UInt256 a = POP();
                UInt256_not(&a);
                PUSH(a);
                break;
            }

            case OP_BYTE: {
                UInt256 x = POP();
                int i = POP().elements[0];

                // Index starting from most significant byte moving backwards
                // if index is in range
                UInt256 y = i > 31 ? ZERO : UInt256_from((uint64_t)*(((uint8_t*)&x.elements[3]) - i));

                PUSH(y);
                break;
            }

            case OP_SHL: {
                UInt256 value = POP();
                uint32_t shift = (uint32_t)POP().elements[3];
                UInt256_shiftleft(&value, shift);
                PUSH(value);
                break;
            }

            case OP_SHR: {
                UInt256 value = POP();
                uint32_t shift = (uint32_t)POP().elements[3];
                UInt256_shiftright(&value, shift);
                PUSH(value);
                break;
            }

            case OP_SAR: {
                UInt256 value = POP();
                uint32_t shift = (uint32_t)POP().elements[3];

                UInt256_shiftright(&value, shift);

                /* If negative by 2's compliment, then
                shifted bits become 1 instead of 0 */
                if (UInt256_get(&value, 0)) {
                    value.elements[0] |= (ULLONG_MAX << (64 - shift));
                    value.elements[1] |= (ULLONG_MAX << (64 - (shift - 64)));
                    value.elements[2] |= (ULLONG_MAX << (64 - (shift - 128)));
                    value.elements[3] |= (ULLONG_MAX << (64 - (shift - 192)));
                }

                PUSH(value);

                break;
            }

            case OP_SHA3: {
                uint64_t size = POP().elements[3], offset = POP().elements[3];

                SHA3_CTX ctx;
                Keccak_init(&ctx);
                Keccak_update(&ctx, Memory_offset(&vm->memory, offset), size);

                uint64_t buffer[4];
                Keccak_final(&ctx, (uint8_t*)&buffer);

                /* TODO: buffer may have to be reversed?
                (either byte or bit wise) */
                UInt256 hash = (UInt256){ { buffer[0], buffer[1], buffer[2], buffer[3] } };

                PUSH(hash);
                
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
                push(UInt256_from(caller_address));
                break;
            }

            case OP_CALLVALUE: {
                error("Unhandled opcode CALLVALUE\n");
                break;
            }

            case OP_CALLDATALOAD: {
                UInt256 i = POP();
                PUSH(UInt256_from(*(uint64_t*)&calldata[i.elements[3]]));
                break;
            }

            case OP_CALLDATASIZE: {
                PUSH(UInt256_from(calldata_size));
                break;
            }

            case OP_CALLDATACOPY: {
                UInt256 size = POP(), offset = POP(), destOffset = POP();
                Memory_insert(&vm->memory, destOffset.elements[3], calldata, size.elements[3]);
                break;
            }

            case OP_CODESIZE: {
                PUSH(UInt256_from(contract->code_size));
                break;
            }

            case OP_CODECOPY: {
                UInt256 size = POP(), offset = POP(), destOffset = POP();
                Memory_insert(&vm->memory, destOffset.elements[3], contract->code, size.elements[3]);
                break;
            }

            case OP_GASPRICE: {
                error("Unhandled opcode GASPRICE\n");
                break;
            }

            case OP_EXTCODESIZE: {
                UInt256 address = POP();
                PUSH(UInt256_from(vm->contracts[address.elements[3]]->code_size));
                break;
            }

            case OP_EXTCODECOPY: {
                UInt256 size = POP(), offset = POP(), dest_offset = POP(), address = POP();
                Memory_insert(&vm->memory, dest_offset.elements[3], vm->contracts[address.elements[3]], size.elements[3]);
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
                POP(); // Throw away value
                break;
            }

            /* BM: Byte array operations are little-endian */

            case OP_MLOAD: {
                uint64_t offset = POP().elements[3];
                uint64_t *mem = (uint64_t*)Memory_offset(&vm->memory, offset);
                UInt256 value = (UInt256){ { mem[3], mem[2], mem[1], mem[0] } };
                PUSH(value);
                break;
            }

            case OP_MSTORE: {
                UInt256 value = POP();
                uint64_t offset = POP().elements[3];
                
                // Reverse and store in buffer for
                // writing little-endian to memory
                uint64_t buffer[] = { value.elements[3], value.elements[2], value.elements[1], value.elements[0] };

                Memory_insert(&vm->memory, offset, (uint8_t*)&buffer[0], 32);

                break;
            }

            case OP_MSTORE8: {
                UInt256 value = POP();
                uint64_t offset = POP().elements[3];
                Memory_insert(&vm->memory, offset, (uint8_t*)&value.elements[3], 8);
                break;
            }

            case OP_SLOAD: {
                UInt256 key = POP(), value;
                UInt256_copy(Storage_get(&vm->storage, &key), &value);
                PUSH(value);
                break;
            }

            case OP_SSTORE: {
                UInt256 value = POP(), key = POP();
                Storage_insert(&vm->storage, &key, &value);
                break;
            }

            case OP_JUMP: {
                UInt256 counter = POP();
                size_t new_pc = (size_t)counter.elements[3];
                if (contract->code[new_pc] == OP_JUMPDEST) pc = new_pc;
                else error("Expected JUMP instruction to jump to JUMPDEST, got %zu\n", pc);
                break;
            }

            case OP_JUMPI: {
                UInt256 b = POP(), counter = POP();
                size_t new_pc = counter.elements[3];
                if (!UInt256_equals(&b, &ZERO)) {
                    if (contract->code[new_pc] == OP_JUMPDEST) pc = new_pc;
                    else error("Expected JUMPI instruction to jump to JUMPDEST, got %zu\n", pc);
                }
                break;
            }

            case OP_PC: {
                PUSH(UInt256_from(pc - 1));
                break;
            }

            case OP_MSIZE: {
                // TODO: Not sure about starting memory capacity/expansion?
                PUSH(UInt256_from(vm->memory.capacity));
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
                uint64_t length = opcode - OP_PUSH1 + 1;

                UInt256 value = ZERO;
                uint8_t *buffer = (uint8_t*)&value.elements[0] + 7; // Last byte in smallest word

                for (int i = 0; i < length; i++)
                    *(buffer - i) = consume_byte(vm);
                
                PUSH(value);
                
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
                    stack[i] = stack[i - 1];
                stack[0] = stack[stack_offset + 1];
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
                UInt256 tmp = stack[0];
                stack[0] = stack[swap_index];
                stack[swap_index] = tmp;
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

            case OP_CREATE:
            case OP_CREATE2: {
                // TODO: Maybe handle contract addresses differently
                // for CREATE/CREATE2 and actually use `salt`?

                // Pop unused `salt` parameter if CREATE2
                if (opcode == OP_CREATE2) /* _salt = */ POP();

                UInt256 size = POP(), _value = POP(), offset = POP();

                size_t code_size = size.elements[3];
                uint8_t *code = malloc(code_size);

                for (size_t i = 0; i < code_size; i++)
                    code[i] = vm->memory.array[offset.elements[3] + i];

                Contract *contract = (Contract*)malloc(sizeof(Contract));
                
                contract->code = code;
                contract->code_size = code_size;

                // Add contract to local buffer and push index
                PUSH(UInt256_from(add_contract(&vm, contract))); 

                break;
            }

            case OP_CALL: 
            case OP_CALLCODE:
            case OP_DELEGATECALL: 
            case OP_STATICCALL: {
                /* TODO: This block currently only handles CALL;
                add some branching for handling nuances of CALLCODE,
                DELEGATECALL, and STATICCALL. */

                UInt256 ret_size = POP(), ret_offset = POP(), args_size = POP(),
                    args_offset = POP(), _value = POP(), address = POP(), _gas = POP();

                Contract *contract = vm->contracts[address.elements[3]];
                VM contract_vm;

                uint8_t *return_buffer;
                size_t return_buffer_size;

                VM_init(&contract_vm);
                
                // TODO: Figure out how passing contract addresses should work

                // TODO: Handle reverts and status codes
                VM_call(&contract_vm, contract, 0 /* TENTATIVE */, Memory_offset(&vm->memory, args_offset.elements[3]), args_size.elements[3], &return_buffer, &return_buffer_size);

                // TODO: Figure out how `ret_size` should work w.r.t `return_buffer_size`?
                
                // Insert return data into memory
                Memory_insert(&vm->memory, ret_offset.elements[3], return_buffer, ret_size.elements[3]);

                break;
            }

            case OP_RETURN: {
                UInt256 size = POP(), offset = POP();

                /* Allocate new buffer, fill with return data, then
                set passed pointer parameters `return_buffer` to point to buffer
                and `return_buffer_size` to point to size of buffer. */

                size_t return_buffer_size = size.elements[3];
                uint8_t* return_buffer = malloc(return_buffer_size);

                for (size_t i = 0; i < size.elements[3]; i++)
                    return_buffer[i] = vm->memory.array[offset.elements[3] + i];
                
                *out_return_buffer = return_buffer;
                *out_return_buffer_size = return_buffer_size;

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
