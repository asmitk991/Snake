#ifndef STRING_H_CUSTOM
#define STRING_H_CUSTOM

/* string functions without string.h */

int  my_strlen(const char *s);
void my_strcpy(char *dst, const char *src);
int  my_strcmp(const char *a, const char *b);
void my_itoa(int val, char *buf);          /* integer to string */
int  my_atoi(const char *s);               /* string to integer */

#endif
