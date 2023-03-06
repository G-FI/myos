#ifndef KHEAP_H_
#define KHEAP_H_

#include <stdint.h>
#include "ordered_array.h"
#include "pmm.h"

uint32_t kmalloc(uint32_t sz);
uint32_t kmalloc_a(uint32_t sz);
//(uint32_t)*pa is address
uint32_t kmalloc_ap(uint32_t sz, uint32_t *pa);
uint32_t kmalloc_p(uint32_t sz, uint32_t *pa);


#define KHEAP_START 0xC0000000
#define KHEAP_MAX_ADDR 0xCFFFF000
#define KHEAP_INITIAL_SIZE 0x100000 //include header_tabler of size 0x8000 
#define KHEAP_MIN_SIZE 0x70000
#define KHEAP_INDEX_SIZE 0x2000
#define MAGIC 0x12345678


typedef struct{
    uint32_t magic;
    uint32_t size;
    int    is_hole;
}header_t;

typedef struct{
    uint32_t magic;
    header_t *header;
}footer_t;

typedef struct{
    ordered_array_t headers_ptr; //空闲的hole的有序数组，当一个hole被使用时就要从这个里面remove，当创建hole时需要insert
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t max_addr;
    int supervisor;
    int writable;
} heap_t;


heap_t* create_heap(uint32_t start, uint32_t end, uint32_t max, int supervisor, int writable);
void* alloc(uint32_t sz, int aligned, heap_t *heap);
void* free(void *p, heap_t *heap);



#endif //KHEAP_H_
