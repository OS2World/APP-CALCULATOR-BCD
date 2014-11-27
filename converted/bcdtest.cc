#include "bcd.h"
#include <stdio.h>
#include <time.h>

int main(void) {
#if 1
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
printf("%f\n", (end-start)/(double)CLOCKS_PER_SEC);
#else
	char abuf[200];
	char bbuf[200];
	bcd a,b;
bcd2a(a,abuf); bcd2a(b,bbuf);
printf("a: %s   b:%s\n",abuf,bbuf);
	a = 17;
bcd2a(a,abuf); bcd2a(b,bbuf);
printf("a: %s   b:%s\n",abuf,bbuf);
	b = 42.3;
bcd2a(a,abuf); bcd2a(b,bbuf);
printf("a: %s   b:%s\n",abuf,bbuf);
	a += b;
bcd2a(a,abuf); bcd2a(b,bbuf);
printf("a: %s   b:%s\n",abuf,bbuf);
	b /= a;
bcd2a(a,abuf); bcd2a(b,bbuf);
printf("a: %s   b:%s\n",abuf,bbuf);
#endif
	return 0;
}
