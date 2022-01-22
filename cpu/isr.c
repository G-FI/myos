#include "isr.h"
#include "ports.h"
#include "../libc/string.h"
#include "../drivers/screen.h"


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
    kprint("receive interrupt: ");
    char buf[8];
    int_to_ascii(regs.int_no, buf);
    kprint(buf);
    kprint(" -> ");
    kprint(exception_messages[regs.int_no]);
    kprint("\n");
}

void irq_handler(registers_t regs){
   // Send an EOI (end of interrupt) signal to the PICs.
   // If this interrupt involved the slave.
    if(regs.int_no >= 40) port_byte_out(0xA0, 0x20);
  
    port_byte_out(0x20, 0x20);
   
    kprint("in irq_handler\n");
   if(interrupt_handlers[regs.int_no] != 0){
        interrupt_handlers[regs.int_no](regs);
    }
}



