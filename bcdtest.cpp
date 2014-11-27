#include "bcd.hpp"
#include <stdio.h>
#include <time.h>

void main(void) {
clock_t start=clock();
        bcd a,b,c;
        bcd d(4200);
        for(int i=1; i<8000; i++) {
                a = 12.5;
                b = 3;
                c = a*b;
                c/=i;
                c+=a;
                c+=b;
                c*=d;
        }
clock_t end=clock();
printf("%f\n", (end-start)/(double)CLK_TCK);
}

