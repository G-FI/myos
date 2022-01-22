#ifndef MEM_H_
#define MEM_H_
#include <stddef.h>
#include <stdint.h>

void mem_copy(char *src, char *dst, int n);
uint32_t kmalloc(int size, uint32_t *pa);

#endif //MEM_H_