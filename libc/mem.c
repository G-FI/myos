#include "mem.h"
#include "../drivers/screen.h"
#include "string.h"
void mem_copy(char *src, char *dst, int n){
    for(int i = 0; i < n; i++){
        dst[i] = src[i];
    }
}


/*kernel code start at 0x1000, it will be fine at 0x10000 to allocate memory*/
uint32_t free_memory = 0x10000;
int npage;

uint32_t kmalloc(int size, uint32_t *pa){
    //keep free_memory aligned before every kmalloc finish
   if(pa != 0){
        *pa = free_memory;
   }
    kprint("\nfreememory:");
    char buf[32];
    hex_to_ascii(free_memory, buf);
    kprint(buf);
    npage++;

    free_memory += size;
    if((free_memory & 0xFFFF) != 0){
        free_memory += 0x1000;
        free_memory &= 0xFFFF0000;
    }
    return npage - 1;
}

