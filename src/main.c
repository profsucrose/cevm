#include <time.h>
#include <assert.h>

#include "storage.h"
#include "uint256.h"

int main() {
    #define X 1385175
    #define Y 2

    UInt256 *x = UInt256_pfrom(X);
    UInt256 *y = UInt256_pfrom(Y);

    UInt256_print(x);
    printf("\n");
    __PRINT(y);
    printf("\n");

    UInt256_pow(x, y);

    __PRINT(x);
    printf("\n");
}