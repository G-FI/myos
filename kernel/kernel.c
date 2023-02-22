#include "kernel.h"
#include "../cpu/descriptor_tables.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "../drivers/keyboard.h"
#include "../libc/printf.h"
#include "vmm.h"
#include "../libc/function.h"
//test
#include "kheap.h"
#include "ordered_array.h"

extern heap_t* kheap;

void test_find_hole(){
    int i = find_smallest_hole(FRAMESZ, 0, kheap);
    ASSERT(i == 0);

    extract(KHEAP_MIN_SIZE, kheap);
    ASSERT(kheap->end_addr == kheap->start_addr + KHEAP_MIN_SIZE);
    //char *p = kheap->start_addr + KHEAP_MIN_SIZE;
    //char c = *p; //will cause pagefault
    expand(KHEAP_MIN_SIZE + 1, kheap);
    ASSERT(kheap->end_addr == kheap->start_addr + KHEAP_MIN_SIZE + FRAMESZ);
}

void test_heap_alloc(){
   char* ptr1 = (char*)alloc(492, 0, kheap);
   char* ptr2 = (char*)alloc(1004, 0, kheap);
   char* ptr3 = (char*)alloc(1234, 1, kheap);
   char* ptr4 = (char*)alloc(0x100000, 0, kheap);
}

header_t *get_header(uint32_t p){
    return p-sizeof(header_t);
}


void kernel_main(){
    init_descriptor_tables();

    asm volatile ("sti");
    init_timer(500);
    init_keyboard();
    initialize_paging();
    

    while(1);
}

