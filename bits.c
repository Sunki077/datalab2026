#include "bits.h"

int bitAnd(int x, int y) {
    return ~(~x | ~y);
}

int bitXor(int x, int y) {
    return ~(~(x | y) | ~(~x | ~y));
}

int samesign(int x, int y) {
    if (x == 0 && y == 0)
        return 1;

    if (x == 0 || y == 0)
        return 0;

    return ((x < 0) == (y < 0));
}

int logtwo(int v) {
    int r = 0;
    if (v >> 16) {
        r += 16;
        v >>= 16;
    }
    if (v >> 8) {
        r += 8;
        v >>= 8;
    }
    if (v >> 4) {
        r += 4;
        v >>= 4;
    }
    if (v >> 2) {
        r += 2;
        v >>= 2;
    }
    if (v >> 1)
        r++;
    return r;
}

int byteSwap(int x, int n, int m) {
    int ns = n << 3;
    int ms = m << 3;
    int bn = (x >> ns) & 255;
    int bm = (x >> ms) & 255;
    int mask = (255 << ns) | (255 << ms);

    return (x & ~mask) | (bn << ms) | (bm << ns);
}

unsigned reverse(unsigned v) {
    v = ((v & 0x55555555u) << 1) |
        ((v >> 1) & 0x55555555u);

    v = ((v & 0x33333333u) << 2) |
        ((v >> 2) & 0x33333333u);

    v = ((v & 0x0f0f0f0fu) << 4) |
        ((v >> 4) & 0x0f0f0f0fu);

    return (v << 24) |
           ((v & 0x0000ff00u) << 8) |
           ((v >> 8) & 0x0000ff00u) |
           (v >> 24);
}

int logicalShift(int x, int n) {
    return (unsigned)x >> n;
}

int leftBitCount(int x) {
    unsigned u = (unsigned)x;

    if (u == 0xffffffffu)
        return 32;

    int n = 0;

    if ((u >> 16) == 0xffff) {
        n += 16;
        u <<= 16;
    }

    if ((u >> 24) == 0xff) {
        n += 8;
        u <<= 8;
    }

    if ((u >> 28) == 0xf) {
        n += 4;
        u <<= 4;
    }

    if ((u >> 30) == 0x3) {
        n += 2;
        u <<= 2;
    }

    if (u >> 31)
        n++;

    return n;
}

unsigned float_i2f(int x) {
    if (x == 0)
        return 0;

    unsigned sign = 0;
    unsigned u = (unsigned)x;

    if (x < 0) {
        sign = 0x80000000u;
        u = (unsigned)(-x);
    }

    int e = 31;
    while (!((u >> e) & 1))
        e--;

    unsigned exponent = (unsigned)(e + 127) << 23;
    unsigned fraction;

    if (e <= 23) {
        fraction = (u << (23 - e)) & 0x7fffff;
    } else {
        int shift = e - 23;
        unsigned significand = u >> shift;
        unsigned remainder = u & ((1u << shift) - 1);
        unsigned halfway = 1u << (shift - 1);

        fraction = significand & 0x7fffff;

        if (remainder > halfway ||
            (remainder == halfway && (fraction & 1))) {
            fraction++;

            if (fraction >> 23) {
                exponent += 0x800000;
                fraction = 0;
            }
        }
    }

    return sign | exponent | fraction;
}

unsigned floatScale2(unsigned uf) {
    unsigned sign = uf & 0x80000000u;
    unsigned exponent = uf & 0x7f800000u;
    unsigned fraction = uf & 0x007fffffu;

    if (exponent == 0x7f800000u)
        return uf;

    if (exponent == 0)
        return sign | (fraction << 1);

    exponent += 0x00800000u;

    if (exponent == 0x7f800000u)
        fraction = 0;

    return sign | exponent | fraction;
}

int float64_f2i(unsigned uf1, unsigned uf2) {
    unsigned low = uf1;
    unsigned high = uf2;

    unsigned sign = high >> 31;
    unsigned exponent = (high >> 20) & 0x7ff;
    unsigned fraction = high & 0xfffff;

    if (exponent < 1023)
        return 0;

    if (exponent >= 1054)
        return (int)0x80000000u;

    unsigned long long mantissa =
        ((unsigned long long)(fraction | 0x100000) << 32) | low;

    int shift = (int)exponent - 1075;
    unsigned long long value;

    if (shift >= 0)
        value = mantissa << shift;
    else
        value = mantissa >> (-shift);

    if (value > 0x7fffffffULL + (sign ? 1 : 0))
        return (int)0x80000000u;

    return sign ? -(int)value : (int)value;
}

unsigned floatPower2(int x) {
    if (x < -149)
        return 0;

    if (x < -126)
        return 1u << (x + 149);

    if (x > 127)
        return 0x7f800000u;

    return (unsigned)(x + 127) << 23;
}