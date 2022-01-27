#include "vmm.h"
#include "pmm.h"
#include "kheap.h"
#include "../libc/string.h"
#include "../libc/printf.h"

extern uint32_t *frames;
extern uint32_t nframes;
extern uint32_t placement_address;

pagetable_dir_t *kernel_dir;
pagetable_dir_t *cur_dir;

extern heap_t *kheap;


void initialize_paging(){
    nframes = MEM_END/FRAMESZ;
    frames = (char*)kmalloc_a(nframes / 8);
    memset(frames, 0, nframes/8); //nframes is number of bits

    
    //make a kernel pagetable_dir
    uint32_t kernel_dir_addr;
    kernel_dir = (pagetable_dir_t *)kmalloc_ap(sizeof(pagetable_dir_t), &kernel_dir_addr);

    memset(kernel_dir, 0, sizeof(pagetable_dir_t));

    //map kheap to kernel_dir, but don't allocate frame, in order to 
    //direct map of first kernelpagetable,
    //then the heap frame is palced after kernel data struct(page table)
    for(uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += FRAMESZ)
        get_pte(i, 1, kernel_dir);

    uint32_t pa = 0;
    //add one frame for kernel heap_t struct, which will 
    //be allocated in creat heap, after that placement_address
    //will be invalid
    while(pa < placement_address + FRAMESZ){
        //get_page return pte of a table, and alloc_frame map it to a frame
        alloc_frame(get_pte(pa, 1, kernel_dir), 0, 0);
        pa += FRAMESZ;
    }

     for(uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += FRAMESZ)
        alloc_frame(get_pte(i, 1, kernel_dir), 0, 1);

    registe_interrupt_handler(14, page_fault);

    //enable page
    switch_page_dir(kernel_dir);

    kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDR, 0, 1);
}

void switch_page_dir(pagetable_dir_t *new_dir){
    //cr3 store the address of talbe pointer?
    asm volatile("mov %0, %%cr3"::"r"(new_dir->table_physical)); 
    uint32_t cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));    
    cr0 |= 0x80000000;//enable paging
    asm volatile("mov %0, %%cr0"::"r"(cr0));

    printf("swich_page ok\n");
}

pte_t *get_pte(uint32_t pa, int make, pagetable_dir_t *dir){
    uint32_t frame = (pa>>12);
    uint32_t table_idx = frame / 1024;
    uint32_t pte_idx = frame % 1024;
    if(dir->tables[table_idx]){
        return &dir->tables[table_idx]->ptes[pte_idx];

    }else if(make){
        uint32_t addr;
        dir->tables[table_idx] = (pagetable_t*)kmalloc_ap(sizeof(pagetable_t), &addr);
        memset(dir->tables[table_idx], 0, sizeof(pagetable_t));
        //physical addr of the new pagetable
        dir->table_physical[table_idx] = addr | 0x7; //user | rw | present 
        return &dir->tables[table_idx]->ptes[pte_idx];
    }
    PANIC("get_pte");
}

void page_fault(registers_t regs){
    uint32_t fault_address;
    asm volatile("mov %%cr2, %0":"=r"(fault_address));
    int p = regs.err_code & 0x1;
    int w = regs.err_code & 0x2;
    int u = regs.err_code & 0x4;
    int id = regs.err_code &0x10;
    
    printf("%p", fault_address);
    if(!p)printf(", present");
    if(w) printf(", wirte fault");
    if(u) printf(", user mode");
    if(id) printf(", fetch instructor");
    printf("\n");

    PANIC("page fault");
}

