#ifndef LOGS_H
#define LOGS_H

#include "common.h"

typedef struct {
    uint8_t *data;
    size_t size;

    size_t topics_length;
    UInt256 *topics;
} Log;

typedef struct {
    size_t capacity;
    size_t length;
    Log **elements;
} Logs;

void Logs_init(Logs *logs);
void Logs_push(Logs *logs, Log *log);

#endif