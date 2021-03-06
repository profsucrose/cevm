#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#include "uint256.h"
#include "bigint/bigint.h"
#include "keccak/keccak256.h"

// TODO: Better error handling
#define error(args...) fprintf(stderr, args); exit(1);

#endif