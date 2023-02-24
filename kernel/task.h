#ifndef TASK_H_
#define TASK_H_
#include "vmm.h"

typedef struct task
{
    int id;
    uint32_t esp, ebp; 
    uint32_t eip;
    pagetable_dir_t* page_directory;
    struct task* next;
}task_t;

void initialise_tasking();

void switch_task();

int fork();

void move_stack(void* new_stack_start, uint32_t sz);

int getpid();


#endif //TASK_H_