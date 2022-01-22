#ifndef STRING_H_
#define STRING_H_

void int_to_ascii(int n, char *buf);
void reverse(char *str);
int  strlen(char *str);
int  strcmp(char *s1, char *s2);
void append(char *str, char c);
void backspace(char *buf);
void hex_to_ascii(int n, char *buf);

#endif //STRING_H_