#ifndef common_h
#define common_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#include "bigint/bigint.h"

#define BigInt_equals(a, b) (BigInt_compare(a, b) == 0)

#endif