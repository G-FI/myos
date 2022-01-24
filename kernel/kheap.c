#include "kheap.h"
#include "pmm.h" //for FRAMESZ

extern uint32_t end;

uint32_t placement_address = (uint32_t*)&end;

//pre-declaration
static uint32_t kheap_aux(uint32_t sz, int aligned, uint32_t *pa);

uint32_t kmalloc(uint32_t sz){
    return kheap_aux(sz, 0, 0);
}
uint32_t kmalloc_a(uint32_t sz){
    return kheap_aux(sz, 1, 0);
}
uint32_t kmalloc_ap(uint32_t sz, uint32_t *pa){
    return kheap_aux(sz, 1, pa);
}
uint32_t kmalloc_p(uint32_t sz, uint32_t *pa){
    return kheap_aux(sz, 0, pa);
}

//if aligned, the start address of allocated should be aligned
uint32_t kheap_aux(uint32_t sz, int aligned, uint32_t *pa){
    if(aligned){
        if(placement_address & 0xFFF){
            placement_address += FRAMESZ;
            placement_address &= 0xFFFFF000;
        }
    }

    if(pa){
        *pa = placement_address;
    }
    uint32_t start_addr = placement_address;
    placement_address += sz;
    return start_addr;
}