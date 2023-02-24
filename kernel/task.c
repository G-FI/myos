#include "task.h"
#include <stdint.h>
#include "kheap.h"
#include "vmm.h"
#include "pmm.h"
#include "../libc/string.h"

extern pagetable_dir_t* kernel_dir;
extern pagetable_dir_t* cur_dir;
extern uint32_t initial_esp;
extern uint32_t read_eip();

volatile task_t *current_task;
volatile task_t* ready_queue;

uint32_t next_pid = 1;

void initialise_tasking(){
    asm volatile("cli");

    //将栈移动位置
    move_stack((void*)0xE0000000, 0x2000);

    //初始化第一个task，kernel task
    current_task = ready_queue = (task_t*)kmalloc(sizeof(task_t));
    current_task->ebp = current_task->esp = 0;
    current_task->eip = 0;
    current_task->id = next_pid++;
    current_task->page_directory = cur_dir;
    current_task->next = 0;

    asm volatile("sti");
}

int fork(){
    asm volatile("cli");
    task_t* tmp_task = ready_queue;
    uint32_t esp; 
    uint32_t ebp;
    uint32_t eip;

    task_t* parent_task = (task_t*)current_task;
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));
    pagetable_dir_t* directory = clone_directory(parent_task->page_directory);
    
    new_task->id = next_pid++;
    new_task->page_directory = directory;
    new_task->next = 0;
    new_task->ebp = new_task->esp = 0;
    new_task->eip = 0;

    while(tmp_task->next){
        tmp_task = tmp_task->next;
    }

    tmp_task->next = new_task;

    // This will be the entry point for the new process.
    eip = read_eip();


    // We could be the parent or the child here - check.
    if (current_task == parent_task)
    {
        // We are the parent, so set up the esp/ebp/eip for our child.
        asm volatile("mov %%esp, %0" : "=r"(esp));
        asm volatile("mov %%ebp, %0" : "=r"(ebp));
        new_task->esp = esp;
        new_task->ebp = ebp;
        new_task->eip = eip;
        asm volatile("sti");

        return new_task->id;
    }
    else
    {
        // We are the child.
        return 0;
    }
    }

void switch_task(){
    //还未初始化task
    if(!current_task)
        return;
    asm volatile("cli");
    
    //获取栈指针，
    uint32_t esp, ebp, eip;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));

    eip = read_eip();

    if(eip == 0x12345)
        return;

    //保存当前进程的状态信息
    //若current_task被再次调度之后，他会跳转到上面的read_eip()之后的指令位置，
    //但时此时需要从switch_task中返回,
    //当切换下一个任务时，给read_eip的返回值eip设置为一个特殊值
    //检查若是这个值说明是已经切换后的进程，需要返回
    current_task->eip = eip; 
    current_task->ebp = ebp;
    current_task->esp = esp;

    //切换下一进程
    current_task = current_task->next;
    if(!current_task) current_task = ready_queue;

    cur_dir = current_task->page_directory; 
    esp = current_task->esp;
    ebp = current_task->ebp;
    eip = current_task->eip;

    //将cpu状态设置为将要执行的task的状态
    //并跳转到task的eip位置
    //在跳转之前，给eax赋值为0x12345
    //因为该switch的目的地只有两个，一个是上面的read_eip的下一条指令，一个是fork中read_eip的下一条指令
    //在fork中可以通过task进行判断是子进程开始父进程
    //而在此函数中，可以通过给eip赋特殊值来判断
     asm volatile("         \
      cli;                 \
      mov %0, %%ecx;       \
      mov %1, %%esp;       \
      mov %2, %%ebp;       \
      mov %3, %%cr3;       \
      mov $0x12345, %%eax; \
      sti;                 \
      jmp *%%ecx           "
                 : : "r"(eip), "r"(esp), "r"(ebp), "r"(cur_dir->physical_addr));
}


int getpid(){
    return current_task->id;
}

void move_stack(void *new_stack_start, uint32_t sz){

    //创建新的栈区，并将栈区映射到页表中
    for(uint32_t i = (uint32_t)new_stack_start; i >= (uint32_t)new_stack_start - sz; i-= FRAMESZ){
        alloc_frame(get_pte(i, 1, cur_dir), 0, 1);
    }

    //通过读写 cr3寄存器刷新TLB
    uint32_t pd_addr;
    asm volatile("mov %%cr3, %0" : "=r" (pd_addr));
    asm volatile("mov %0, %%cr3" : : "r" (pd_addr));
    
    //获取原来的esp 和 ebp
    uint32_t old_stack_pointer; asm volatile("mov %%esp, %0" : "=r" (old_stack_pointer));
    uint32_t old_base_pointer;  asm volatile("mov %%ebp, %0" : "=r" (old_base_pointer));

    uint32_t offset = (uint32_t)new_stack_start - initial_esp;
    
    uint32_t new_stack_pointer = old_stack_pointer + offset;
    uint32_t new_base_pointer = old_base_pointer + offset;

    // Copy the stack.
    memcpy((void*)new_stack_pointer, (void*)old_stack_pointer, initial_esp-old_stack_pointer);

    // Backtrace through the original stack, copying new values into
    // the new stack.  
    for(uint32_t i = (uint32_t)new_stack_start; i > (uint32_t)new_stack_start-sz; i -= 4)
    {
        uint32_t tmp = * (uint32_t*)i;
        // If the value of tmp is inside the range of the old stack, assume it is a base pointer
        // and remap it. This will unfortunately remap ANY value in this range, whether they are
        // base pointers or not.
        if (( old_stack_pointer < tmp) && (tmp < initial_esp))
        {
        tmp = tmp + offset;
        uint32_t *tmp2 = (uint32_t*)i;
        *tmp2 = tmp;
        }
    }

    // Change stacks.
    asm volatile("mov %0, %%esp" : : "r" (new_stack_pointer));
    asm volatile("mov %0, %%ebp" : : "r" (new_base_pointer));
}