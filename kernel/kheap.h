#ifndef KHEAP_H_
#define KHEAP_H_

#include <stdint.h>

uint32_t kmalloc(uint32_t sz);
uint32_t kmalloc_a(uint32_t sz);
//(uint32_t)*pa is address
uint32_t kmalloc_ap(uint32_t sz, uint32_t *pa);
uint32_t kmalloc_p(uint32_t sz, uint32_t *pa);


#endif //KHEAP_H_
