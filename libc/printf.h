#ifndef PRINTF_H_
#define PRINTF_H_

void printf(char*, ...);
void panic(char*, char*, char*)__attribute__((noreturn));
#define PANIC(msg) do{panic(msg, __FILE_, __LINE__)}while(0)
#endif //PRINTF_H