#include <time.h>
#include <assert.h>
#include <keccak/keccak256.h>

#include "vm.h"

int main() {
    srand(time(NULL));

    VM vm;
    VM_init(&vm);

    Logs logs;
    Logs_init(&logs);

    uint8_t code[] = { 
        OP_PUSH1, 102, // [ value ]
        OP_PUSH1, 0, // [ value, offset ]
        OP_MSTORE8,     // []
        OP_PUSH1, 1, // [ size ]
        OP_PUSH1, 0, // [ size, offset ]
        OP_LOG0,        // []
        OP_STOP,
    };

    Context context = (Context){
        .code = code,
        .code_size = sizeof(code),

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