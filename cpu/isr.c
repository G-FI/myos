#include "isr.h"
#include "ports.h"
#include "../libc/string.h"
#include "../libc/printf.h"


isr_t interrupt_handlers[256];

//public API to set hardware interrupt
void registe_interrupt_handler(uint8_t n, isr_t handler){
    interrupt_handlers[n] = handler;
}

/* To print the message which defines every exception */
char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};


//cpu产生异常处理程序dispatch
void isr_handler(registers_t regs){
    if(interrupt_handlers[regs.int_no]){
        interrupt_handlers[regs.int_no](regs);
    }else{
        printf("unhandled exception: %s\n", exception_messages[regs.int_no]);
    }
}

//中断处理程序dispath，根据出错的interrupt_number来查找中断处理程序，并执行
void irq_handler(registers_t regs){
   // Send an EOI (end of interrupt) signal to the PICs.
   // If this interrupt involved the slave.
    if(interrupt_handlers[regs.int_no] != 0){
        interrupt_handlers[regs.int_no](regs);
    }else{
        printf("unhandled interrupt: %d\n", regs.int_no);
    }
    if(regs.int_no >= 40) port_byte_out(0xA0, 0x20);
  
    port_byte_out(0x20, 0x20);
   
}



