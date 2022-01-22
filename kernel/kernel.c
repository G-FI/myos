#include "kernel.h"
#include "../cpu/descriptor_tables.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "../drivers/keyboard.h"
#include "../libc/printf.h"

void kernel_main(){
    init_descriptor_tables();
    asm volatile ("sti");
    init_timer(5);
    init_keyboard();
}


void user_input(char *buf){
    if(strcmp(buf, "END") == 0){
        kprint("STOPING THE CPU, BYE\n");
        asm volatile("hlt");
    }else if(strcmp(buf, "MALLOC") == 0){
        kprint("TESTING MALLOC:\n");
        uint32_t pa;
        int pg = kmalloc(1, &pa);
        char pg_buf[8];
        int_to_ascii(pg, pg_buf);
        char pa_buf[32];
        hex_to_ascii(pa, pa_buf);

        kprint("PAGE NUMBER: ");
        kprint(pg_buf);
        kprint(", PHYSICIAL ADDRESS: ");
        kprint(pa_buf);
        kprint("\n");
    }
    kprint("got input: ");
    kprint(buf);
    kprint("\n>");
}