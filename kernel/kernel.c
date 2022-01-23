#include "kernel.h"
#include "../cpu/descriptor_tables.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "../drivers/keyboard.h"
#include "../libc/printf.h"
#include "pmm.h"


void kernel_main(){
    init_descriptor_tables();

    asm volatile ("sti");
    init_timer(500);
    init_keyboard();
    initialize_pmm();
    while(1);
}
