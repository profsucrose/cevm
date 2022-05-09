#ifndef memory_h
#define memory_h

#include "common.h"

typedef struct {
    uint8_t *array;
    uint64_t capacity;
    uint64_t length;
} Memory;

void Memory_init(Memory *memory);
void Memory_insert(Memory *memory, uint64_t index, uint8_t *buffer, size_t length);
uint8_t *Memory_get(Memory *memory, uint64_t index);

#endif