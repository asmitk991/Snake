#include "string.h"
#include "math.h"

int my_strlen(const char *s) {
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

void my_strcpy(char *dst, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

int my_strcmp(const char *a, const char *b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return a[i] - b[i];
        i++;
    }
    return a[i] - b[i];
}

/* convert a number to a string */
void my_itoa(int val, char *buf) {
    int i = 0;
    int neg = 0;

    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    if (val < 0) {
        neg = 1;
        val = -val;
    }
    /* fill it backwards first */
    char tmp[20];
    int  len = 0;
    while (val > 0) {
        tmp[len++] = '0' + my_mod(val, 10);
        val = my_div(val, 10);
    }
    if (neg) buf[i++] = '-';
    /* flip it the right way */
    for (int j = len - 1; j >= 0; j--) buf[i++] = tmp[j];
    buf[i] = '\0';
}

int my_atoi(const char *s) {
    int result = 0;
    int neg = 0;
    int i = 0;
    if (s[i] == '-') { neg = 1; i++; }
    while (s[i] >= '0' && s[i] <= '9') {
        result = my_mul(result, 10) + (s[i] - '0');
        i++;
    }
    return neg ? -result : result;
}
