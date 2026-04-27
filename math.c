#include "math.h"

/* Fast multiplication using bit-shifts */
int my_mul(int a, int b) {
    int res = 0;
    int neg = 0;
    if (a < 0) { a = -a; neg = !neg; }
    if (b < 0) { b = -b; neg = !neg; }

    while (b > 0) {
        if (b & 1) res += a;
        a <<= 1;
        b >>= 1;
    }
    return neg ? -res : res;
}

/* Fast division using bit-shifts */
int my_div(int a, int b) {
    if (b == 0) return 0;
    int res = 0;
    int neg = 0;
    unsigned int ua = (unsigned int)(a < 0 ? -a : a);
    unsigned int ub = (unsigned int)(b < 0 ? -b : b);
    if ((a < 0) ^ (b < 0)) neg = 1;

    for (int i = 31; i >= 0; i--) {
        if ((ua >> i) >= ub) {
            ua -= (ub << i);
            res |= (1 << i);
        }
    }
    return neg ? -res : res;
}

/* Get remainder */
int my_mod(int a, int b) {
    if (b == 0) return 0;
    int quotient = my_div(a, b);
    return a - my_mul(quotient, b);
}

/* Absolute value */
int my_abs(int a) {
    return a < 0 ? -a : a;
}

/* Clamp value within bounds */
int my_clamp(int val, int min, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

/* Check if value is within bounds */
int my_inbounds(int val, int min, int max) {
    return (val >= min && val < max) ? 1 : 0;
}
