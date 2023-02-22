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
    cur_dir = kernel_dir;
    //先分配kheap的页表，但是不分配空间，只是分配页表
    for(uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += FRAMESZ)
        get_pte(i, 1, kernel_dir);

    uint32_t pa = 0;
    
    //映射从零开始到heap_t数据结构的内存，作为直接映射，也就是分页情况下在heap_t数据结构以下的内存，虚拟地址==物理地址
    //并且初始化frame管理器
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
    //在kheap创建之前使用的物理空间都是placement_address，进行分配，
    //堆区空间分配在了页表直接上去的地址处，但是虚拟地址却在KHEAP_START那里

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

//只是得到va对应的page_table的pte，如果那个teable存在的话，pte要么也存在，要么不存在
//当不存在时就是要去alloc_frame，get_pte只负责得到那个pte，而不管哪个pte是否有对应的frame
pte_t *get_pte(uint32_t va, int make, pagetable_dir_t *dir){
    uint32_t frame = (va>>12);
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

