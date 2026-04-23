#ifndef MATH_H
#define MATH_H

/* basic math without headers */

int  my_mul(int a, int b);
int  my_div(int a, int b);
int  my_mod(int a, int b);
int  my_abs(int a);
int  my_clamp(int val, int min, int max);
int  my_inbounds(int val, int min, int max);  /* 1 if val in [min,max), else 0 */

#endif
