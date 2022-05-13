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

#define BigInt_equals(a, b) (BigInt_compare(a, b) == 0)

#endif