#include "bcd.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N (digits/2)

inline void bcd::nnz() {
        //convert -0.0 to (+)0.0
        if(d[0]==0 && iszero()) d[0]=1;
}        

bcd& bcd::operator=(long l) {
        if(l<0) {
                l=-l;
                *this = (unsigned long)l;
                d[0]=0;
        } else
                *this = (unsigned long)l;
        return *this;        
}
bcd& bcd::operator=(unsigned long ul) {
        memset(d,0,sizeof(d));
        unsigned o=(digits-precision)/2-1;
        while(ul) {
                unsigned char v = (unsigned char)(ul%100);
                ul/=100;
                d[o--] = v;
        }
        d[0]=1;
        return *this;
}

bcd& bcd::operator=(long double ld) {
        memset(d,0,sizeof(d));
        if(ld<0)
                d[0]=0;
        else
                d[0]=1;
        if(ld<0.0) ld=-ld;        
        int i;
        for(i=0; i<precision; i+=2)
                ld*=100;
        char buf[digits];
        sprintf(buf,"%*.0Lf",digits-precision,ld);
        unsigned o=N-1;
        char *p=buf; while(*p) p++;
        p-=2;
        do {
                unsigned v=0;
                if(p[0]>='0' && p[0]<='9')
                        v += (p[0]-'0');
                v*=10;
                if(p[1]>='0' && p[1]<='9')
                        v += (p[1]-'0');
                d[o--] = (unsigned char)v;
                p-=2;
        } while(p>=buf);
        return *this;
}

bcd bcd::operator-() const {
        //sign is located at d[0]
        bcd r(*this);
        if(!r.iszero())
                r.d[0] = 1-r.d[0];
        return r;        
}

int bcd::compare(const bcd &b) const {
        return memcmp(d,b.d,sizeof(d));
}

bcd& bcd::operator+=(const bcd &b) {
        if(d[0]) {
                if(b.d[0]) {
                        //+ +
                        //add code
                        unsigned carry=0;
                        for(unsigned o=N-1; o>0; o--) {
                                unsigned v=d[o];
                                v+=b.d[o];
                                v+=carry;
                                if(v>100) {
                                        carry=1;
                                        v-=100;
                                } else
                                        carry=0;
                                d[o] = (unsigned char)v;
                        }
                } else {
                        //+ -
                        *this -= -b;
                }
        } else {
                if(b.d[0]) {
                        //- +
                        *this = -*this;
                        *this -= b;
                        *this = -*this;
                } else {
                        //- -
                        *this = -*this;
                        *this += -b;
                        *this = -*this;
                }
        }
        return *this;
}

bcd& bcd::operator-=(const bcd &b) {
        if(d[0]) {
                if(b.d[0]) {
                        //+ +
                        //add code here
                        unsigned carry=0;
                        for(unsigned o=N-1; o>0; o--) {
                                int v=d[o];
                                v-=b.d[o];
                                v-=carry;
                                if(v<0) {
                                        carry=1;
                                        v+=100;
                                } else
                                        carry=0;
                                d[o] = (unsigned char)v;        
                        }
                        if(carry)
                                d[0]=0;
                        else
                                d[0]=1;
                } else {
                        //+ -
                        *this += -b;
                }
        } else {
                if(b.d[0]) {
                        //- +
                        *this = -*this;
                        *this += b;
                        *this = -*this;
                } else {
                        //- -
                        *this += -b;
                }
        }
        return *this;
}

bcd& bcd::operator*=(bcd b) {
        bcd a(*this);
        if(a.iszero() || b.iszero()) {
                *this = 0;
                return *this;
        }
        int muldiv10=precision;
        while(a.d[N-1]%10==0) {
                muldiv10--;
                a.div10();
        }
        while(b.d[N-1]%10==0) {
                muldiv10--;
                b.div10();
        }
        
        if(a<b) { bcd tmp=a; a=b; b=tmp; }

        bcd r=0;
        while(!b.iszero()) {
                unsigned m=b.d[N-1]%10;
                if(m) {
                        //r+=a*m
                        bcd tmp=a;
                        tmp.mul1digit(m);
                        r+= tmp;
                }
                b.div10();
                a.mul10();
        }
        if(muldiv10<0) {
                muldiv10 = -muldiv10;
                for(int i=0; i<muldiv10; i++)
                        r.mul10();
        } else if(muldiv10==0) {
        } else {
                for(int i=0; i<muldiv10; i++)
                        r.div10();
        }        
        *this = r;
        return *this;
}

bcd& bcd::operator/=(bcd B) {
        if(iszero())
                return *this;
        
        bcd q(0), r(*this);
        int sign;
        if(d[0])
                if(B.d[0])
                        sign=1;
                else
                        sign=0, B=-B;
        else
                if(B.d[0])
                        sign=0, r=-r;
                else
                        sign=1, r=-r, B=-B;
        int muldiv10=0;
        //shl r to improve precision
        while(r.d[1]==0) {
                r.mul10();
                muldiv10--;
        }        
 
        bcd b(B);
        //now the method taught in primary school
        while(r>=b)
                b.mul10();
        while(b!=B) {
                q.mul10();
                b.div10();
                
                unsigned long m;
                wholediv(r,b,&m);
                q += m;
                r -= b*m;
        }
        //set sign
        q.d[0] = (unsigned char)sign;
        //adjust precision
        if(muldiv10<0) {
                while(muldiv10++)
                        q.div10(muldiv10==0);
        } else if(muldiv10==0) {
        } else if(muldiv10>0) {
                while(muldiv10--)
                        q.mul10();
        }
        *this = q;
        return *this;
}

bcd bcd::frac() const {
        bcd r(*this);
        memset(r.d+1, 0, sizeof(d[0])*(digits-precision)/2-1);
        return r;
}

bcd bcd::trunc() const {
        bcd r(*this);
        memset(r.d+(digits-precision)/2, 0, sizeof(d[0])*(precision/2));
        return r;
}

int bcd::iszero() const {
        for(unsigned o=1; o<digits/2; o++)
                if(d[o]) return 0;
        return 1;        
}

void bcd::mul10() {
        unsigned o;
        for(o=1; o<N-1;  o++)
                d[o] = (d[o]%10)*10 + d[o+1]/10;
        d[o] = (d[o]%10)*10;
}

void bcd::div10(int round) {
        unsigned o;
        int r=d[N-1]%10;
        for(o=N-1; o>1; o--)
                d[o] = d[o]/10 + (d[o-1]%10)*10;
        d[o] /= 10;
        if(round && r>=5) {
                o=N-1;
                for(;;) {
                        d[o]++;
                        if(d[o]>=100) {
                                d[o] -= 100;
                                o--;
                        } else
                                break;
                }                
        }
}

void bcd::mul1digit(unsigned m) {
        unsigned carry=0;
        for(unsigned o=N-1; o>0; o--) {
                unsigned v=d[o];
                v*=m;
                v+=carry;
                if(v>100) {
                        carry = v/100;
                        v%= 100;
                } else
                        carry = 0;
                d[o] = (unsigned char)v;
        }        
}

void bcd::wholediv(const bcd &a, const bcd &b, unsigned long *m) {
        bcd tmp(a);
        *m=0;
        while(tmp>=b) {
                tmp -= b;
                ++*m;
        }
}

int sgn(const bcd &a) {
        if(a.iszero())
                return 0;
        if(a.d[0])
                return 1;
        else
                return -1;
}

char *bcd2a(const bcd &b, char *buf) {
        char *savebuf=buf;
        if(b.d[0]==0)
                *buf++ = '-';
        unsigned o;
        int a=0;
        for(o=1; o<(digits-precision)/2; o++) {
                int d=b.d[o]/10;
                if(d || a) {
                        *buf++ = '0'+d;
                        a=1;
                }
                if(o==(digits-precision)/2-1) a=1;
                d=b.d[o]%10;
                if(d || a) {
                        *buf++ = '0'+d;
                        a=1;
                }        
        }
        *buf++ = '.';
        for(; o<N; o++) {
                int d=b.d[o]/10;
                *buf++ = '0'+d;
                d=b.d[o]%10;
                *buf++ = '0'+d;
        }
        *buf++ = '\0';
        return savebuf;
}

bcd a2bcd(const char *s) {
        while(*s==' ') s++;
        bcd w(0);
        int negative;
        if(*s=='-') {
                negative=1;
                s++;
        } else {
                if(*s=='+') s++;
                negative=0;
        }
        
        while(*s && *s!=' ' && *s!='.') {
                int d=*s-'0';
                w.mul10();
                w+=d;
                s++;
        }
        
        bcd f(0);
        if(*s=='.') {
                s++;
                bcd p(1);
                while(*s && *s!=' ' && !p.iszero()) {
                        p.div10();
                        int d=*s-'0';
                        f += p*d;
                        s++;
                }
        }
        return w+f;
}

