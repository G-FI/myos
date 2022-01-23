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


void isr_handler(registers_t regs){
    printf("receive interrupt: %d -->%s\n", regs.int_no,
                    exception_messages[regs.int_no]);
}

void irq_handler(registers_t regs){
   // Send an EOI (end of interrupt) signal to the PICs.
   // If this interrupt involved the slave.

    if(interrupt_handlers[regs.int_no] != 0){
        interrupt_handlers[regs.int_no](regs);
    }
    if(regs.int_no >= 40) port_byte_out(0xA0, 0x20);
  
    kprint("in irq_handler\n");
    port_byte_out(0x20, 0x20);
   
}



