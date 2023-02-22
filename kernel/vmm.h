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
    //table
    pagetable_t *tables[ENTRIES];
    
    //array of pointers to physical addr of tables
    //当clonetable时用的上
    uint32_t table_physical[ENTRIES];

    uint32_t physical_addr;

}pagetable_dir_t;

//设置环境，页表，开启分页
void initialize_paging();

//将新的页目录加载到CR3寄存器
void switch_page_dir(pagetable_dir_t *new_dir);

//根据虚拟地址获取对应的页表项pte，若make设置为1，若第二级页表不存在则创建T
pte_t *get_pte(uint32_t pa, int make, pagetable_dir_t *dir);

void page_fault(registers_t regs);





#endif //VMM_H_