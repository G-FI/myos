#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
//high 4 bits is bg_color, low 4bits is fg_color
#define WHITH_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

//screen I/O port
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
uint16_t vga_entry(unsigned char uc, uint8_t color);

/*TODO print msg with customed color
void kprint(char *msg, uint8_t color);*/
void kprint_at(char *msg, int row, int col);
void kprint(char *msg);
int kprint_char(char c, int row, int col, char attr);
void kprint_backspace();
void clear_screen();

#endif //SCREEN_H_