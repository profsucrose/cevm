#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <keccak/keccak256.h>
#include "vm.h"

const char *program = "608060405234801561001057600080fd5b5061017c806100206000396000f3fe608060405234801561001057600080fd5b506004361061002b5760003560e01c8063c605f76c14610030575b600080fd5b61003861004e565b6040516100459190610124565b60405180910390f35b60606040518060400160405280600d81526020017f48656c6c6f2c20576f726c642100000000000000000000000000000000000000815250905090565b600081519050919050565b600082825260208201905092915050565b60005b838110156100c55780820151818401526020810190506100aa565b838111156100d4576000848401525b50505050565b6000601f19601f8301169050919050565b60006100f68261008b565b6101008185610096565b93506101108185602086016100a7565b610119816100da565b840191505092915050565b6000602082019050818103600083015261013e81846100eb565b90509291505056fea2646970667358221220ce6cc94ce286d0931a98df4f00040eb03e2ea63ebae695416170c2acd6584c2064736f6c63430008090033";

uint8_t hex_to_dec(char ch) {
    return isdigit(ch) 
        ? (uint8_t)ch - (uint8_t)'0'
        : (uint8_t)ch - (uint8_t)'a' + 10;
}

uint8_t *hex_to_opcodes(const char *hex, size_t *length) {
    size_t code_length = strlen(hex) / 2; // Every 2 hex chars is one byte

    uint8_t *code = malloc(code_length);

    size_t char_i;
    char c1, c2;

    for (size_t i = 0; i < code_length; i++) {
        char_i = i * 2;

        c1 = hex[char_i], c2 = hex[char_i + 1];

        code[i] = hex_to_dec(c1) * 16 + hex_to_dec(c2);
    }

    *length = code_length;

    return code;
}

int main() {
    srand(time(NULL));

    VM vm;
    VM_init(&vm);

    Logs logs;
    Logs_init(&logs);

    uint8_t code[] = { 
        OP_PUSH1, 0x66,  // [ value ]
        OP_PUSH1, 0x00,  // [ value, offset ]
        OP_MSTORE8,      // []
        OP_PUSH1, 0x01,  // [ size ]
        OP_PUSH1, 0x00,  // [ size, offset ]
        OP_LOG0,         // []
        OP_STOP,
    };

    size_t hello_world_sol_length;
    uint8_t *hello_world_sol = hex_to_opcodes(program, &hello_world_sol_length);

    for (size_t i = 0; i < hello_world_sol_length; i++) {
        uint8_t opcode = hello_world_sol[i];

        const char *name = OPCODE_TO_NAME[hello_world_sol[i]];
        printf("%s\n", name);

        if (opcode >= OP_PUSH1 && opcode <= OP_PUSH32) {
            printf("Is push opcode\n");
            size_t values = opcode - OP_PUSH1 + 1;

            printf("0x");

            // Each value is 2 hex chars
            for (size_t j = 0; j < values * 2; j++) {
                char ch = program[i * 2 + j];
                printf("%c", ch);
            }

            printf("\n");

            i += values / 2 + 1;
        }
    }

    Context context = (Context){
        .code = hello_world_sol,
        .code_size = hello_world_sol_length,

        .address = 0,

        .sender = 0,

        .stack_top = context.stack,

        .memory = (Memory*)malloc(sizeof(Memory)),

        .storage = (Storage*)malloc(sizeof(Storage)),

        .calldata = NULL,
        .calldata_size = 0,

        .return_data = NULL,
        .return_data_size = 0,
    };

    Memory_init(context.memory);
    Storage_init(context.storage);

    VM_call(&vm, &context, &logs);

    printf("Logs length: %zu\n", logs.length);

    for (size_t i = 0; i < logs.length; i++) {
        Log *log = logs.elements[i];
        printf("LOG%d:\n", (int)log->topics_length);

        printf("  topics:\n");

        for (size_t j = 0; j < log->topics_length; j++) {
            printf("    "); UInt256_print(&log->topics[j]); printf("\n");
        }

        printf("  data: (%d)\n", (int)log->size);

        for (size_t j = 0; j < log->size; j++) {
            printf("    %d\n", (int)log->data[j]);
        }
    }
}