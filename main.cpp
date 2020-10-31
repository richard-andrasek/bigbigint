


#include "BigBigInt.h"
#include <memory.h>
#include <stdio.h>

void unsigned_divide(
            unsigned int dividend,
            unsigned int divisor,
            unsigned int *quotient,
            unsigned int *remainder );

int main(){
    bigbigint myint, myint2;

    myint = 0xFFFFFFF1;
    myint2 = 0xFFFFFFFF;

    myint2 = myint << 2;

    return 0;
}