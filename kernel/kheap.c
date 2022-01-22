#include "kheap.h"

extern uint32_t end;
uint32_t placement_address = &(end);

uint32_t kmalloc(uint32_t sz){
    uint32_t tmp = placement_address;
    placement_address += sz;
    return tmp;
}