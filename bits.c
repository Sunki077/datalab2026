#include "bits.h"

int bitAnd(int x, int y) {
    return ~(~x | ~y);
}

int bitXor(int x, int y) {
    return ~(~x & ~y) & ~(x & y);
}

int samesign(int x, int y) {
    if (!x)
        return !y;

    if (!y)
        return 0;

    return !((x ^ y) >> 31);
}

int logtwo(int v) {
    int r;
    int s;

    r = 0;

    s = ((v >> 16) > 0) << 4;
    r |= s;
    v >>= s;

    s = ((v >> 8) > 0) << 3;
    r |= s;
    v >>= s;

    s = ((v >> 4) > 0) << 2;
    r |= s;
    v >>= s;

    s = ((v >> 2) > 0) << 1;
    r |= s;
    v >>= s;

    return r | (v >> 1);
}

int byteSwap(int x, int n, int m) {
    int ns;
    int ms;
    int bn;
    int bm;
    int mask;

    ns = n << 3;
    ms = m << 3;
    bn = (x >> ns) & 255;
    bm = (x >> ms) & 255;
    mask = (255 << ns) | (255 << ms);

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
    int mask;

    mask = ~(((1 << 31) >> n) << 1);
    return (x >> n) & mask;
}

int leftBitCount(int x) {
    int b16;
    int b8;
    int b4;
    int b2;
    int b1;
    int last;

    b16 = !((~x) >> 16) << 4;
    x <<= b16;

    b8 = !((~x) >> 24) << 3;
    x <<= b8;

    b4 = !((~x) >> 28) << 2;
    x <<= b4;

    b2 = !((~x) >> 30) << 1;
    x <<= b2;

    b1 = !((~x) >> 31);
    x <<= b1;

    last = (x >> 31) & 1;

    return b16 + b8 + b4 + b2 + b1 + last;
}

unsigned float_i2f(int x) {
    unsigned sign;
    unsigned magnitude;
    unsigned exponent;
    unsigned fraction;
    unsigned remainder;

    if (!x)
        return 0;

    sign = x & 0x80000000u;
    magnitude = x;

    if (sign)
        magnitude = ~magnitude + 1;

    exponent = 158;

    while (!(magnitude & 0x80000000u)) {
        magnitude <<= 1;
        exponent--;
    }

    fraction = (magnitude >> 8) & 0x007fffffu;
    remainder = magnitude & 0xffu;

    if (remainder > 0x80u)
        fraction = fraction + 1;

    if (remainder == 0x80u) {
        if (fraction & 1u)
            fraction = fraction + 1;
    }

    if (fraction >> 23) {
        fraction &= 0x007fffffu;
        exponent += 1;
    }

    return sign | (exponent << 23) | fraction;
}

unsigned floatScale2(unsigned uf) {
    unsigned sign;
    unsigned exponent;
    unsigned fraction;

    sign = uf & 0x80000000u;
    exponent = uf & 0x7f800000u;
    fraction = uf & 0x007fffffu;

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
    unsigned low;
    unsigned high;
    unsigned sign;
    unsigned exponent;
    unsigned mantissa;
    unsigned value;
    int e;

    low = uf1;
    high = uf2;
    sign = high >> 31;
    exponent = (high >> 20) & 0x7ffu;
    mantissa = (high & 0xfffffu) | 0x100000u;

    if (exponent < 1023u)
        return 0;

    if (exponent > 1053u)
        return 0x80000000u;

    e = exponent - 1023u;

    if (e <= 20) {
        value = mantissa >> (20 - e);
    } else {
        value = (mantissa << (e - 20)) |
                (low >> (52 - e));
    }

    if (sign)
        return ~value + 1;

    return value;
}

unsigned floatPower2(int x) {
    if (x < -149)
        return 0;

    if (x < -126)
        return 1u << (x + 149);

    if (x > 127)
        return 0x7f800000u;

    return (x + 127) << 23;
}