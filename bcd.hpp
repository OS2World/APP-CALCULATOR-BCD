#ifndef __BCD_HPP
#define __BCD_HPP

class bcd {
public:
        enum { digits=24, precision=4 };
        //Enough for number like
        //+000.000.000.000.000.000,0000
        
        bcd()  {}
        bcd(int i)
          { *this=i; }
        bcd(long l)
          { *this=l; }
        bcd(unsigned long ul)
          { *this=ul; }
        bcd(float f)
          { *this=f; }
        bcd(double d)
          { *this=d; }
        bcd(long double ld)
          { *this=ld; }
        
        bcd& operator=(int i)
          { return *this=(long)i; }
        bcd& operator=(double d)
          { return *this=(long double)d; }
        bcd& operator=(long l);
        bcd& operator=(unsigned long ul);
        bcd& operator=(long double ld);
        
        int compare(const bcd &b) const;
        
        int operator<(const bcd &b) const
          { return compare(b)<0; }
        int operator<=(const bcd &b) const
          { return compare(b)<=0; }
        int operator==(const bcd &b) const
          { return compare(b)==0; }
        int operator>=(const bcd &b) const
          { return compare(b)>=0; }
        int operator>(const bcd &b) const
          { return compare(b)>0; }
        int operator!=(const bcd &b) const
          { return compare(b)!=0; }
        
        bcd& operator+=(const bcd &b);
        bcd& operator-=(const bcd &b);
        
        bcd& operator*=(bcd b);
        bcd& operator/=(bcd b);
        
        bcd operator-() const;
        
        bcd frac() const;
        bcd trunc() const;

        void mul10();
        void mul100();
        void div10(int round=0);
        void div100(int round=0);
        int  iszero() const;
        void mul1digit(unsigned m);
private:
        static void wholediv(const bcd &a, const bcd &b, unsigned long *m);
        unsigned char d[(digits+1)/2];
        friend char *bcd2a(const bcd &b, char *buf);
        friend int sgn(const bcd &a);
        void nnz();
        void shl(int t);
};

inline bcd operator+(const bcd& a, const bcd &b)
  { bcd t(a); t+=b; return t; }
inline bcd operator-(const bcd& a, const bcd &b)
  { bcd t(a); t-=b; return t; }
inline bcd operator*(const bcd& a, const bcd &b)
  { bcd t(a); t*=b; return t; }
inline bcd operator/(const bcd& a, const bcd &b)
  { bcd t(a); t/=b; return t; }
int sgn(const bcd &a);
inline bcd abs(const bcd &a)
  { if(sgn(a)==-1) return -a; else return a; }

char *bcd2a(const bcd &b, char *buf);
bcd a2bcd(const char *s);

#endif
