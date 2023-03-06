#include "keyboard.h"
#include "screen.h"
#include "../cpu/ports.h"
#include "../cpu/isr.h"
#include "../libc/string.h"
#include "../kernel/kernel.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define SC_MAX 57

char input_buf[256];

const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};

        
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};


//TODO: add input buffer
void kerboard_callback(){
    uint8_t scancode = port_byte_in(0x60);
    if(scancode > SC_MAX) return;
    if(scancode == BACKSPACE){
        kprint_backspace();
        // if(strlen(input_buf) != 0){
        //     kprint_backspace();
        //     backspace(input_buf);
        // }
    }else if(scancode == ENTER){
        kprint("\n");
        input_buf[0] = 0;
    }else{
        char c = sc_ascii[scancode];
        char str[2] = {c, 0};
        kprint(str); 
        //add to input buf
        //append(input_buf, c);
    }
}

void init_keyboard(){
    registe_interrupt_handler(IRQ1, kerboard_callback);
}


