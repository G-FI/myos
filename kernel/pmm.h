#ifndef PMM_H_
#define PMM_H_

#include <stdint.h>
#include "vmm.h"

//Macro for pmm
#define FRAMESZ 0x1000 //4096 byte
#define MEM_END 0x1000000   //the size of physical address 16Mib



void alloc_frame(pte_t *pte, int is_kernel, int is_writeable);
void free_frame(pte_t *pte);


#endif //PMM_H_