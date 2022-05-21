#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

typedef struct {
    uint8_t *array;
    uint64_t capacity;
    uint64_t length;
} Memory;

void Memory_init(Memory *memory);
void Memory_insert(Memory *memory, uint64_t offset, uint8_t *buffer, size_t length);
uint8_t *Memory_offset(Memory *memory, uint64_t offset);
void Memory_copy(const Memory *src, Memory *dest);
void Memory_free(Memory *memory);
void Memory_move(Memory *from, Memory *to);

#endif