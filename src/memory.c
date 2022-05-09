#include "memory.h"

#define DEFAULT_CAPACITY 1024
#define GROWTH_FACTOR 2

void Memory_init(Memory *memory) {
    memory->capacity = DEFAULT_CAPACITY;
    memory->array = (uint8_t*)calloc(sizeof(uint8_t), memory->capacity);
    memory->length = 0;
}

static void resize(Memory *memory) {
    uint64_t old_capacity = memory->capacity;

    memory->capacity = memory->capacity * GROWTH_FACTOR;
    memory->array = realloc(memory->array, sizeof(uint8_t) * memory->capacity);

    // Zero out new memory
    for (int i = old_capacity; i < memory->capacity; i++)
        memory->array[i] = 0;
}

void Memory_insert(Memory *memory, uint64_t index, uint8_t *buffer, size_t length) {
    // TODO: Maybe return if resized?
    if (index + length > memory->length)
        resize(memory);

    for (size_t i = 0; i < length; i++)
        memory->array[index + i] = buffer[i];
}

uint8_t *Memory_get(Memory *memory, uint64_t index) {
    return memory->array + index;
}