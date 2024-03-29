#include "ports.h"

uint8_t port_byte_in(uint16_t port){
    unsigned char result;
    //in %ax, %dx; from %dx read data to ax
    //the port number must be in dx
    __asm__("in %%dx, %%al": "=a"(result):"d"(port));
    return result;
}

void port_byte_out(uint16_t port, uint8_t data){
    //Transfers byte in AL,word in AX or dword in EAX to the specified hardware port address
    //out port, data
    __asm__("out %%al, %%dx": : "a"(data), "d"(port));
}

uint16_t port_word_in(uint16_t port){
    unsigned short result;
    __asm__("in %%dx, %%ax": "=a"(result): "d"(port));
    return result;
}

void port_wrod_out(uint16_t port, uint16_t data){
    __asm__("out %%ax, %%dx":: "a"(data), "d"(port));
}