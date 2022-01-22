#ifndef PORTS_H_
#define PORTS_H_

#include <stdint.h>

//read a byte from I/O port
uint8_t port_byte_in(uint16_t port);
//write a byte to I/O port
void port_byte_out(uint16_t port, uint8_t data);
//read a word 
uint16_t port_word_in(uint16_t port);
//write a word 
void port_word_out(uint16_t port, uint16_t data);

#endif //PORTS_H_
