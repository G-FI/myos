#include "printf.h"
#include "../drivers/screen.h"
#include "string.h"

#include <stdarg.h>

static char digits[] = "0123456789abcdef";

static void printint(int n, int base){
    char buf[16];
    int sign, i=0;
    if((sign = n) < 0) n = -n;

    do{
        buf[i++] = digits[n%base];
    }while((n /= base) > 0);

    if(sign < 0){
        buf[i++] = '-';
    }
    buf[i] = 0;
    reverse(buf);
    kprint(buf);
}

static void printptr(uint32_t ptr){
    kprint_char('0', -1, -1, 0);
    kprint_char('x', -1, -1, 0);
    for(int i = 28; i >= 0; i-=4){
        kprint_char(digits[(ptr >> i) & 0xF], -1, -1, 0);
    }
}

//formated print, only understand %d %x %p %s
void printf(char *fmt, ...){
    va_list ap;
    char c;
    char *s;

    va_start(ap, fmt);
    for(int i = 0; fmt[i] != 0; i++){
        if(fmt[i] != '%'){
            kprint_char(fmt[i], -1, -1, 0);
            continue;
        }
        c = fmt[++i];
        switch(c){
        case 'd':
            printint(va_arg(ap, int), 10);
            break;
        case 'x':
            printint(va_arg(ap, int), 16);
            break;
        case 'p':
            printptr(va_arg(ap, uint32_t));
            break;
        case 's':
            s = va_arg(ap, char*);
            if(s == 0)
                s = "(null)";
            kprint(s);     
            break;
        default:
            kprint_char('%', -1, -1, 0);
            kprint_char(c, -1, -1, 0);    
            break;
        }
    }
    va_end(ap);
}

void panic(char * msg, char *file, int line){
    printf("%s: %d\n", file, line);
    printf("panic: %s\n", msg);
    for(;;);
}