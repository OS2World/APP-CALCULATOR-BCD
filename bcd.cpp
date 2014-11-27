#include "bcd.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N (bcd::digits/2)

inline void bcd::nnz() {
        //convert (-)0.0 to (+)0.0
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
        if(!r.d[0])
                r.d[0] = 1;
        else if(!r.iszero())
                r.d[0] = 0;
        return r;        
}

int bcd::compare(const bcd &b) const {
        return memcmp(d,b.d,sizeof(d));
}

void addpp(bcd *,const bcd*,unsigned);
#pragma aux addpp=\
"  mov ebx,0            "\
"  mov eax,0            "\
"  mov edx,0            "\
"  dec ecx              "\
"l:                     "\
"  cmp ecx,1            "\
"  je end               "\
"                       "\
"  mov al,[edi+ecx]     "\
"  mov dl,[esi+ecx]     "\
"  add eax,edx          "\
"  add eax,ebx          "\
"  cmp eax,100          "\
"  mov ebx,0            "\
"  jl j2                "\
"  mov ebx,1            "\
"  sub eax,100          "\
"j2:                    "\
"  mov [edi+ecx],al     "\
"                       "\
"  dec ecx              "\
"  jmp l                "\
"end:                   "\
  parm [edi] [esi] [ecx]\
  modify exact [eax ebx ecx edx];

bcd& bcd::operator+=(const bcd &b) {
        if(d[0]) {
                if(b.d[0]) {
                        //+ +
                        addpp(this,&b,N);
/*
                        unsigned char carry=0;
                        for(unsigned o=N-1; o>0; o--) {
                                unsigned char v=d[o];
                                v+=b.d[o];
                                v+=carry;
                                if(v>100) {
                                        carry=1;
                                        v-=100;
                                } else
                                        carry=0;
                                d[o] = (unsigned char)v;
                        }
*/
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

void subpp(bcd *,const bcd*,unsigned);
#pragma aux subpp=\
"  mov ebx,0            "\
"  mov eax,0            "\
"  mov edx,0            "\
"  dec ecx              "\
"l:                     "\
"  cmp ecx,1            "\
"  je end               "\
"                       "\
"  mov al,[edi+ecx]     "\
"  mov dl,[esi+ecx]     "\
"  sub eax,edx          "\
"  sub eax,ebx          "\
"  mov ebx,0            "\
"  jns j2               "\
"  mov ebx,1            "\
"  add eax,100          "\
"j2:                    "\
"  mov [edi+ecx],al     "\
"                       "\
"  dec ecx              "\
"  jmp l                "\
"end:                   "\
"  mov [edi],bl         "\
  parm [edi] [esi] [ecx]\
  modify exact [eax ebx ecx edx];

bcd& bcd::operator-=(const bcd &b) {
        if(d[0]) {
                if(b.d[0]) {
                        //+ +
                        //add code here
                        subpp(this,&b,N);
/*
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
*/
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
        while(a.d[N-1]==0) {
                muldiv10-=2;
                a.div100();
        }        
        if(a.d[N-1]%10==0) {
                muldiv10--;
                a.div10();
        }
        while(b.d[N-1]==0) {
                muldiv10-=2;
                b.div100();
        }        
        if(b.d[N-1]%10==0) {
                muldiv10--;
                b.div10();
        }
        
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
                r.shl(-muldiv10);
        } else if(muldiv10==0) {
        } else {
                while(muldiv10>1) {
                        r.div100(muldiv10==2);
                        muldiv10-=2;
                }        
                if(muldiv10)
                        r.div10(1);
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
        int muldiv10 = precision;
        //shl r to improve precision
        while(r.d[1]==0 && r.d[2]==0) {
                r.mul100();
                muldiv10-=2;
        }        
        while(r.d[1]==0) {
                r.mul10();
                muldiv10--;
        }        
        //shr b to improve precision
        while(B.d[N-1]==0) {
                B.div100();
                muldiv10-=2;
        }        
        bcd b(B);
        //now the method taught in primary school
        //while(r>=b)
        //        b.mul10();
        while(b.d[1]==0 && b.d[2]==0) b.mul100();
        while(r>=b)
                b.mul10();
        
        while(b!=B) {
                q.mul10();
                b.div10();
                
                unsigned long m;
                wholediv(r,b,&m);
                //q += m;
                { bcd tmp(0); tmp.d[N-1]=(unsigned char)m; q+=tmp; }
                r -= b*m;
        }
        //set sign
        q.d[0] = (unsigned char)sign;
        //adjust precision
        if(muldiv10<0) {
                while(muldiv10<-1) {
                        q.div100(muldiv10==-2);
                        muldiv10+=2;
                }        
                if(muldiv10++)
                        q.div10(1);
        } else if(muldiv10==0) {
        } else if(muldiv10>0) {
                q.shl(muldiv10);
        }
        *this = q;
        return *this;
}

bcd bcd::frac() const {
        bcd r(*this);
        memset(r.d+1, 0, sizeof(d[0])*(digits-precision)/2-1);
        r.nnz();
        return r;
}

bcd bcd::trunc() const {
        bcd r(*this);
        memset(r.d+(digits-precision)/2, 0, sizeof(d[0])*(precision/2));
        r.nnz();
        return r;
}

int iszero(const bcd *, int prec);
#pragma aux iszero=\
"  inc edi      "\
"  dec ecx      "\
"  mov eax,0    "\
"  rep scasb    "\
"  je l     "\
"  mov eax,-1    "\
"l:             "\
"  inc eax      "\
  parm [edi] [ecx]\
  modify [eax edi ecx]\
  value [eax];
  
int bcd::iszero() const {
return ::iszero(this,digits/2);
/*
        for(unsigned o=1; o<digits/2; o++)
                if(d[o]) return 0;
        return 1;        
*/
}

void m10(bcd*,unsigned);
#pragma aux m10=\
"  dec ecx              "\
"  add esi,ecx          "\
"  mov bl,0             "\
"                       "\
"  cmp ecx,0            "\
"  je end               "\
"l2:                    "\
"  mov eax,0            "\
"  mov al,[esi]         "\
"  mov dl,10            "\
"  div dl               "\
"  mov bh,al            "\
"  mov al,ah            "\
"  imul al,10           "\
"  add al,bl            "\
"  mov [esi],al         "\
"                       "\
"  mov bl,bh            "\
"                       "\
"  dec esi              "\
"  dec ecx              "\
"  jnz l2               "\
"end:                   "\
  parm [esi] [ecx] \
  modify [eax ebx ecx edx esi];
  
void bcd::mul10() {
m10(this,N);
/*
        unsigned o;
        unsigned char c=0;
        for(o=N-1; o>0; o--) {
                unsigned char nc=d[o]/10;
                d[o] = (d[o]%10)*10 + c;
                c=nc;
        }
*/        
}

void bcd::mul100() {
        memmove(d+1,d+2,N-2);
        d[N-1]=0;
}

void d10(bcd *,unsigned);
#pragma aux d10=\
"  inc esi              "\
"  dec ecx              "\
"  mov bl,0             "\
"  jecxz end            "\
"l:                     "\
"                       "\
"  mov eax,0            "\
"  mov al,[esi]         "\
"  mov dl,10            "\
"  div dl               "\
"  mov bh,ah            "\
"  xchg al,bl           "\
"  imul al,10           "\
"  add al,bl            "\
"  mov [esi],al         "\
"                       "\
"  mov bl,bh            "\
"                       "\
"  inc esi              "\
"  dec ecx              "\
"  jnz l                "\
"end:                   "\
  parm [esi] [ecx]\
  modify [eax ebx ecx edx esi];

void bcd::div10(int round) {
        unsigned o;
        int r=d[N-1]%10;
        d10(this,N);
/*        
        unsigned char c=0;
        for(o=1; o<N; o++) {
                unsigned char nc=d[o]%10;
                d[o] = c*10 + d[o]/10;
                c=nc;
        }
*/
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

void bcd::div100(int round) {
        int r=d[N-1]/100;
        memmove(d+2,d+1,N-2);
        d[1]=0;
        if(round && r>=5) {
                unsigned o=N-1;
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

void bcd::shl(int t) {
        if(t>=digits) {
                *this=0;
        } else {
                unsigned o=t/2;
                if(o) {
                        memmove(d+1,d+1+o,N-1-o);
                        memset(d+N-o,0,o);
                }
                if(t%2)
                        mul10();
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
        for(o=1; o<(bcd::digits-bcd::precision)/2; o++) {
                int d=b.d[o]/10;
                if(d || a) {
                        *buf++ = '0'+d;
                        a=1;
                }
                if(o==(bcd::digits-bcd::precision)/2-1) a=1;
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

