#include "vm.h"

void VM_init(VM *vm) {
    /* Contracts */
    vm->contracts_length = 0;
}

static size_t add_contract(VM *vm, Contract *contract) {
    vm->contracts[vm->contracts_length++] = contract;
    return vm->contracts_length - 1;
}

/* -2^255 in 2's compliment is 1 */
static UInt256 MINUS_UINT256_LIMIT = (UInt256){ { 0, 0, 0, 1 } };
static UInt256 MINUS_ONE = (UInt256){ { ULLONG_MAX, ULLONG_MAX, ULLONG_MAX, ULLONG_MAX } };

bool VM_call(VM *vm, Context *ctx, Logs *out_logs) {
    /* Copy Storage and Memory for reverting state */
    Storage old_storage;
    Storage_copy(ctx->storage, &old_storage);

    Memory old_memory;
    Memory_copy(ctx->memory, &old_memory);

    size_t pc = 0;

    /* Copy UInt256 for stack operations */
    #define POP() (*(--ctx->stack_top))
    #define PUSH(value) *(ctx->stack_top++) = value

    OpCode opcode;

    for (;;) {
        opcode = ctx->code[pc++];
        printf("Processing %d\n", opcode);
        switch (opcode) {
            case OP_STOP: {
                return true; /* Successfully terminate */
            }

            case OP_ADD: {
                UInt256 a = POP(), b = POP();
                UInt256_add(&a, &b);
                PUSH(a);
                break;
            }

            case OP_MUL: {
                UInt256 a = POP(), b = POP();
                UInt256_mult(&a, &b);
                PUSH(a);
                break;
            }

            case OP_SUB: {
                UInt256 a = POP(), b = POP();
                UInt256_sub(&a, &b);
                PUSH(a);
                break;
            }

            case OP_DIV: {
                UInt256 a = POP(), b = POP();
                if (UInt256_equals(&a, &ZERO)) a = ZERO;
                else UInt256_div(&a, &b);
                PUSH(a);
                break;
            }

            case OP_SDIV: { 
                UInt256 a = POP(), b = POP();

                if (UInt256_equals(&b, &ZERO)) a = ZERO;
                else if (UInt256_equals(&a, &MINUS_UINT256_LIMIT) &&
                        UInt256_equals(&b, &MINUS_ONE))
                    a = MINUS_UINT256_LIMIT;
                else {
                    /* Get absolute value of division and store to op1 */
                    UInt256_div(&a, &b);

                    bool a_negative = UInt256_get(&a, 0);
                    bool b_negative = UInt256_get(&b, 0);

                    if (a_negative + b_negative == 1) {
                        /* If one op is negative and other is positive, */
                        /* then negate division */
                        UInt256_compliment(&a);
                    }
                }

                PUSH(a);

                break;
            }

            case OP_MOD: {
                UInt256 a = POP(), b = POP();
                
                if (UInt256_equals(&b, &ZERO)) a = ZERO;
                else UInt256_rem(&a, &b);

                PUSH(a);

                break;
            }

            case OP_SMOD: {
                UInt256 a = POP(), b = POP();
                
                if (UInt256_equals(&b, &ZERO)) a = ZERO;
                else {
                    UInt256_rem(&a, &b);

                    bool a_negative = UInt256_get(&a, 0);

                    /* If a is negative, then negate modulo */
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
                UInt256 a = POP(), b = POP(), N = POP();

                if (UInt256_equals(&N, &ZERO)) a = ZERO;
                else {
                    UInt256_mult(&a, &b);
                    UInt256_rem(&a, &N);
                }

                PUSH(a);
            }

            case OP_EXP: {
                UInt256 a = POP(), exponent = POP();

                UInt256_pow(&a, &exponent);
                PUSH(a);

                break;
            }
            
            case OP_SIGNEXTEND: {
                UInt256 b = POP(), x = POP();

                int t = 256 - 8 * (UInt256_get(&b, 0) + 1);

                for (int i = 0; i < 255; i++)
                    UInt256_set(&b, i, UInt256_get(&x, i >= t ? t : i));

                PUSH(b);

                break;
            }

            case OP_LT: {
                UInt256 a = POP(), b = POP();
                PUSH(UInt256_lt(&a, &b) ? ONE : ZERO);
                break;
            }

            case OP_GT: {
                UInt256 a = POP(), b = POP();
                PUSH(UInt256_gt(&a, &b) ? ONE : ZERO);
                break;
            }

            case OP_SLT: {
                /* Assert ops are in 2's compliment */
                UInt256 a = POP(), b = POP();

                UInt256 a_abs = a; UInt256_abs(&a_abs);
                UInt256 b_abs = b; UInt256_abs(&b_abs);

                bool abs_lt = UInt256_lt(&a_abs, &b_abs);

                bool a_negative = UInt256_get(&a, 0);
                bool b_negative = UInt256_get(&a, 0);

                bool lt = (!a_negative && !b_negative && abs_lt) ||    /* if a > 0 and b > 0, then true if |a| < |b| */
                    (a_negative && !b_negative) ||                     /* if a < 0 and b > 0, then true              */
                    (a_negative && b_negative && !abs_lt);             /* if a < 0 and b < 0, then true if |a| > |b| */

                PUSH(lt ? ONE : ZERO);

                break;
            }

            case OP_SGT: {
                /* Assert ops are in 2's compliment */
                UInt256 a = POP(), b = POP();

                UInt256 a_abs = a; UInt256_abs(&a_abs);
                UInt256 b_abs = b; UInt256_abs(&b_abs);

                bool abs_gt = UInt256_gt(&a_abs, &b_abs);

                bool a_negative = UInt256_get(&a, 0);
                bool b_negative = UInt256_get(&a, 0);

                bool gt = (!a_negative && !b_negative && abs_gt) ||    /* if a > 0 and b > 0, then |a| > |b| */
                    (!a_negative && b_negative) ||                     /* if a > 0 and b < 0, then true      */
                    (a_negative && b_negative && !abs_gt);             /* if a < 0 and b < 0, then |a| < |b| */

                PUSH(gt ? ONE : ZERO);

                break;
            }

            case OP_EQ: {
                UInt256 a = POP(), b = POP();

                PUSH(UInt256_equals(&a, &b) ? ONE : ZERO);

                break;
            }

            case OP_ISZERO: {
                UInt256 a = POP();
                PUSH(UInt256_equals(&a, &ZERO) ? ONE : ZERO);
                break;
            }

            case OP_AND: {
                UInt256 a = POP(), b = POP();
                UInt256_and(&a, &b);
                PUSH(a);
                break;
            }

            case OP_OR: {
                UInt256 a = POP(), b = POP();
                UInt256_or(&a, &b);
                PUSH(a);
                break;
            }

            case OP_XOR: {
                UInt256 a = POP(), b = POP();
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
                UInt256 _x = POP();
                uint64_t x = TO_UINT64(_x);

                uint64_t i = POP().elements[0];

                /*
                 * Index starting from most significant byte 
                 * moving backwards if index is in range
                 */
                UInt256 y = i > 31 ? ZERO : UInt256_from((uint64_t)*(((uint8_t*)&x) - i));

                PUSH(y);
                break;
            }

            case OP_SHL: {
                uint32_t shift = (uint32_t)POP().elements[3];
                UInt256 value = POP();
                UInt256_shiftleft(&value, shift);
                PUSH(value);
                break;
            }

            case OP_SHR: {
                uint32_t shift = (uint32_t)POP().elements[3];
                UInt256 value = POP();
                UInt256_shiftright(&value, shift);
                PUSH(value);
                break;
            }

            case OP_SAR: {
                uint32_t shift = (uint32_t)POP().elements[3];
                UInt256 value = POP();

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
                uint64_t offset = POP().elements[3], size = POP().elements[3];

                SHA3_CTX sha_ctx;
                Keccak_init(&sha_ctx);
                Keccak_update(&sha_ctx, Memory_offset(ctx->memory, offset), size);

                uint64_t buffer[4];
                Keccak_final(&sha_ctx, (uint8_t*)&buffer);

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
                PUSH(UInt256_from(ctx->sender));
                break;
            }

            case OP_CALLVALUE: {
                error("Unhandled opcode CALLVALUE\n");
                break;
            }

            case OP_CALLDATALOAD: {
                UInt256 i = POP();
                PUSH(UInt256_from(*(uint64_t*)&ctx->calldata[i.elements[3]]));
                break;
            }

            case OP_CALLDATASIZE: {
                PUSH(UInt256_from(ctx->calldata_size));
                break;
            }

            case OP_CALLDATACOPY: {
                size_t dest_offset = TO_SIZE_T(POP()), offset = TO_SIZE_T(POP()), size = TO_SIZE_T(POP());
                Memory_insert(ctx->memory, dest_offset, ctx->calldata + offset, size);
                break;
            }

            case OP_CODESIZE: {
                PUSH(UInt256_from(ctx->code_size));
                break;
            }

            case OP_CODECOPY: {
                size_t dest_offset = TO_SIZE_T(POP()), offset = TO_SIZE_T(POP()), size = TO_SIZE_T(POP());
                Memory_insert(ctx->memory, dest_offset, ctx->code + offset, size);
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
                size_t address = TO_SIZE_T(POP()), dest_offset = TO_SIZE_T(POP()), offset = TO_SIZE_T(POP()), size = TO_SIZE_T(POP());
                Memory_insert(ctx->memory, dest_offset, vm->contracts[address]->code + offset, size);
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
                POP(); /* Throw away value */
                break;
            }

            /* BM: Byte array operations are little-endian */

            case OP_MLOAD: {
                uint64_t offset = POP().elements[3];
                uint64_t *mem = (uint64_t*)Memory_offset(ctx->memory, offset);
                UInt256 value = (UInt256){ { mem[3], mem[2], mem[1], mem[0] } };
                PUSH(value);
                break;
            }

            case OP_MSTORE: {
                uint64_t offset = POP().elements[3];
                UInt256 value = POP();
                
                /* Reverse and store in buffer for
                writing little-endian to memory */
                uint64_t buffer[] = { value.elements[3], value.elements[2], value.elements[1], value.elements[0] };

                Memory_insert(ctx->memory, offset, (uint8_t*)&buffer[0], 32);

                break;
            }

            case OP_MSTORE8: {
                UInt256 _offset = POP(), _value = POP();
                size_t offset = TO_SIZE_T(_offset), value = TO_SIZE_T(_value);
                uint8_t buffer[] = { (uint8_t)value };
                Memory_insert(ctx->memory, offset, buffer, 1);

                printf("value u256: "); __PRINT(&_value); printf("\n");
                printf("offset u256: "); __PRINT(&_offset); printf("\n");
                printf("offset: %d; value: %d\n", (int)offset, (int)value);
                printf("value at mem 0: %d\n", (int)ctx->memory->array[offset]);

                break;
            }

            case OP_SLOAD: {
                UInt256 key = POP(), value;
                UInt256_copy(Storage_get(ctx->storage, &key), &value);
                PUSH(value);
                break;
            }

            case OP_SSTORE: {
                UInt256 key = POP(), value = POP();
                Storage_insert(ctx->storage, &key, &value);
                break;
            }

            case OP_JUMP: {
                UInt256 counter = POP();
                size_t new_pc = (size_t)counter.elements[3];
                if (ctx->code[new_pc] == OP_JUMPDEST) pc = new_pc;
                else error("Expected JUMP instruction to jump to JUMPDEST, got %zu\n", pc);
                break;
            }

            case OP_JUMPI: {
                UInt256 counter = POP(), b = POP();
                size_t new_pc = counter.elements[3];
                if (!UInt256_equals(&b, &ZERO)) {
                    if (ctx->code[new_pc] == OP_JUMPDEST) pc = new_pc;
                    else error("Expected JUMPI instruction to jump to JUMPDEST, got %zu\n", pc);
                }
                break;
            }

            case OP_PC: {
                PUSH(UInt256_from(pc - 1));
                break;
            }

            case OP_MSIZE: {
                PUSH(UInt256_from(ctx->memory->capacity));
                break;
            }

            case OP_GAS: {
                error("Unhandled opcode GAS\n");
                break;
            }

            case OP_JUMPDEST: {
                /* Do nothing */
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
                size_t length = opcode - OP_PUSH1 + 1;

                UInt256 value = ZERO;
                uint8_t *buffer = ((uint8_t*)&value.elements[3]); /* Last byte in smallest word */

                for (size_t i = 0; i < length; i++)
                    *(buffer - i) = ctx->code[pc++];
                
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
                    ctx->stack[i] = ctx->stack[i - 1];
                ctx->stack[0] = ctx->stack[stack_offset + 1];
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
                UInt256 tmp = ctx->stack[0];
                ctx->stack[0] = ctx->stack[swap_index];
                ctx->stack[swap_index] = tmp;
                break;
            }

            case OP_LOG0:
            case OP_LOG1:
            case OP_LOG2:
            case OP_LOG3:
            case OP_LOG4: {
                size_t offset = TO_SIZE_T(POP()), size = TO_SIZE_T(POP());

                size_t topics_length = (size_t)(opcode - OP_LOG0);
                UInt256 *topics = malloc(sizeof(UInt256) * topics_length);

                for (size_t i = 0; i < topics_length; i++)
                    topics[i] = POP();
                
                uint8_t *data = malloc(size);

                for (size_t i = 0; i < size; i++)
                    data[i] = ctx->memory->array[offset + i];
                
                Log *log = malloc(sizeof(Log));

                log->data = data;
                log->size = size;

                log->topics_length = topics_length;
                log->topics = topics;

                Logs_push(out_logs, log);

                break;
            }

            case OP_CREATE:
            case OP_CREATE2: {
                /* TODO: Add predetermined addresses for CREATE2 */

                size_t _value = TO_SIZE_T(POP()), offset = TO_SIZE_T(POP()), size = TO_SIZE_T(POP());

                /* Pop unused `salt` parameter if CREATE2 */
                if (opcode == OP_CREATE2) /* _salt = */ POP();

                size_t code_size = size;
                uint8_t *code = malloc(code_size);

                for (size_t i = 0; i < code_size; i++)
                    code[i] = ctx->memory->array[offset + i];

                Contract *contract = (Contract*)malloc(sizeof(Contract));
                
                contract->code = code;
                contract->code_size = code_size;
                contract->address = add_contract(vm, contract);

                PUSH(UInt256_from(add_contract(vm, contract)));

                break;
            }

            case OP_CALL:  
            case OP_CALLCODE:
            case OP_DELEGATECALL:
            case OP_STATICCALL: {
                UInt256 _gas = POP(), _value; // TODO: Maybe implement some form of gas accounting?
                size_t address = TO_SIZE_T(POP());
                
                /* Only CALL and CALLCODE take a `value` parameter */
                if (opcode == OP_CALL || opcode == OP_CALLCODE) _value = POP();
            
                size_t args_offset = TO_SIZE_T(POP()), args_size = TO_SIZE_T(POP()), return_offset = TO_SIZE_T(POP()), return_size = TO_SIZE_T(POP());

                Contract *contract = vm->contracts[address];

                Context subcontext;

                /*
                 * Populate subcontext, start with shared attributes 
                 * (code, stack, calldata, return data, address of contract to call)
                 */
                subcontext.code = contract->code;
                subcontext.code_size = contract->code_size;

                subcontext.stack_top = subcontext.stack;

                subcontext.calldata = &ctx->memory->array[args_offset];
                subcontext.calldata_size = args_size;

                subcontext.return_data = (uint8_t*)malloc(return_size);
                subcontext.return_data_size = return_size;

                subcontext.address = address;

                if (opcode == OP_CALL) {
                    subcontext.sender = ctx->address;
                    Memory_init(subcontext.memory);
                    subcontext.storage = &contract->storage;
                } else /* OP_CALLCODE || OP_DELEGATECALL */ {
                    subcontext.sender = ctx->sender; /* Sender carries over from current ctx */
                    subcontext.storage = ctx->storage;
                }

                bool status = VM_call(vm, &subcontext, out_logs);
                PUSH(UInt256_from(status));

                /* Insert return data into Memory */
                Memory_insert(ctx->memory, return_offset, subcontext.return_data, return_size);

                free(subcontext.return_data); /* No longer need return data buffer */

                break;
            }

            case OP_RETURN: {
                size_t offset = TO_SIZE_T(POP()), size = TO_SIZE_T(POP());

                uint8_t* return_data = malloc(size);

                for (size_t i = 0; i < size; i++)
                    return_data[i] = ctx->memory->array[offset + i];
                
                ctx->return_data = return_data;
                ctx->return_data_size = size;

                /* Don't need to store state for case of reversion */
                Storage_free(&old_storage);
                Memory_free(&old_memory);

                return true; /* Success */
            }

            case OP_REVERT: {
                /* Revert changes by restoring original state */
                Storage_move(&old_storage, ctx->storage);
                Memory_move(&old_memory, ctx->memory);

                return false;
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
