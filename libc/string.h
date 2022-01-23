#ifndef STRING_H_
#define STRING_H_
#include <stdint.h>

void int_to_ascii(int n, char *buf);
void reverse(char *str);
int  strlen(const char *str);
int  strcmp(const char *s1, const char *s2);
void append(char *str, char c);
void backspace(char *buf);
void hex_to_ascii(int n, char *buf);

void* memcpy(void *dst, const void *src, uint32_t sz);
void* memset(void *dst, int c, uint32_t sz);

#endif //STRING_H_