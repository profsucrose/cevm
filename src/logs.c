#include "logs.h"

#define DEFAULT_CAPACITY 10

static void *recalloc(void *ptr, size_t current_size, size_t new_size) {
    void *new_ptr = realloc(ptr, new_size);

    for (size_t i = current_size; i < new_size; i++)
        ((uint8_t*)ptr)[i] = 0;

    return new_ptr;
}

void Logs_init(Logs *logs) {
    logs->capacity = DEFAULT_CAPACITY;
    logs->elements = calloc(sizeof(Log), logs->capacity);
    logs->length = 0;
}

void Logs_push(Logs *logs, Log *log) {
    logs->elements[logs->length++] = log;

    if (logs->length == logs->capacity) {
        size_t new_capacity = logs->capacity * 2;
        logs->elements = recalloc(logs->elements, sizeof(Log) * new_capacity, logs->capacity);
        logs->capacity = new_capacity;
    }
}