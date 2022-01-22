#ifndef PAGING_H_
#define PAGING_H_

#include <stdint.h>
#include "../cpu/isr.h"

#define ENTRIES 1024

//page
typedef strcut {
    uint32_t present    :1;
    uint32_t rw         :1;
    uint32_t user       :1;
    uint32_t access     :1;
    uint32_t dirty      :1;
    uint32_t unused     :7;
    uint32_t frame      :20;
}page_t;

//pagetable
typedef struct{
    page_t ptes[ENTRIES];
}pagetable_t;

//pagetable directory
typedef struct{
    pagetable_t *tables[ENTRIES];
    
    //array of pointers to physical addr of tables
    uint32_t table_physical[ENTRIES];

    uint32_t phycal_addr;

}pagetable_dir_t;

void initialize_paging();

void switch_page_dir(pagetable_dir_t *new_dir);

pte_t *get_page(uint32_t pa, int make, pagetable_dir_t *dir);

void page_fault(registers_t *regs);





#endif //PAGING_H_