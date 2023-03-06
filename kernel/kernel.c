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

#include "multiboot.h"
#include "initrd.h"
#include "fs.h"
#include "task.h"

extern uint32_t placement_address;
uint32_t initial_esp; //起动栈的位置

//test
/*
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
*/

void initrd_test(fs_node_t* fs_root){
    int i = 0;
    struct dirent *node = 0;
    while ( (node = readdir_fs(fs_root, i)) != 0)
    {
    printf("Found file:");
    printf(node->name);
    fs_node_t *fsnode = finddir_fs(fs_root, node->name);

    if ((fsnode->flags&0x7) == FS_DIRECTORY)
        printf("            (directory)\n");
    else
    {
        printf("              contents:");
        char buf[256];
        uint32_t sz = read_fs(fsnode, 0, 256, buf);
        buf[39] = '\0';
        printf("\"%s\"\n", buf);
    }
    i++;
    }
}


void kernel_main(struct multiboot* mboot_ptr, uint32_t initial_stack){

    initial_esp = initial_stack;
    init_descriptor_tables();

    ASSERT(mboot_ptr->mods_count > 0);
    uint32_t initrd_location = *((uint32_t*)mboot_ptr->mods_addr);
    uint32_t initrd_end = *(uint32_t*)(mboot_ptr->mods_addr+4);
    placement_address = initrd_end;

    asm volatile ("sti");
    init_keyboard();
    initialize_paging();

    fs_root = initialise_initrd(initrd_location);

    initrd_test(fs_root);

    initialise_tasking();
    init_timer(100000000);

    int ret = fork(); 
    printf("the forked process id is %d\n", ret);

    
    while(1);
}

