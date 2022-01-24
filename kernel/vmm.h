#ifndef VMM_H_
#define VMM_H_

#include <stdint.h>
#include "../cpu/isr.h"

#define ENTRIES 1024

//pte, sizeof(pte) = 4 bytes
typedef struct{
    uint32_t present    :1;
    uint32_t rw         :1;
    uint32_t user       :1;
    uint32_t access     :1;
    uint32_t dirty      :1;
    uint32_t unused     :7;
    uint32_t frame      :20;
}pte_t;  


//pagetable: sizeof(pagetable_t) = 4096 bytes
typedef struct{
    pte_t ptes[ENTRIES];
}pagetable_t;

//pagetable directory: sizeof(pagetable_dir_t) = 4096 + 4096 + 4 = 8196 bytes, take 3 physical frames
typedef struct{
    pagetable_t *tables[ENTRIES];
    
    //array of pointers to physical addr of tables
    uint32_t table_physical[ENTRIES];

    uint32_t physical_addr;

}pagetable_dir_t;

void initialize_paging();

void switch_page_dir(pagetable_dir_t *new_dir);

pte_t *get_pte(uint32_t pa, int make, pagetable_dir_t *dir);

void page_fault(registers_t regs);





#endif //VMM_H_