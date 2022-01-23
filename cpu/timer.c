#include "timer.h"
#include "isr.h"
#include "../libc/string.h"
#include "../libc/printf.h"

uint32_t ticks = 0;

static void timer_callback(registers_t regs){
    ++ticks;
    printf("ticks: %d\n", ticks);
}

void init_timer(uint32_t freq){
    //reigster timer interrupt handler
    registe_interrupt_handler(IRQ0, timer_callback);
    uint32_t divisor = 1193180/ freq;
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor>>8)& 0xFF);

    //send the command
    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}