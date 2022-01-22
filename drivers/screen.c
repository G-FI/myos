#include"screen.h"
#include"../cpu/ports.h"
#include "../libc/string.h"
#include"../libc/mem.h"

/*TODO use global variable instead ask vag port*/

//helper function pre-declearation
int get_screen_offset(int row, int col);
int get_cursor_offset();
void set_cursor_offset(int offset);
int get_offset_row(int offset);
int get_offset_col(int offset);
int handle_screen(int offset);
int kprint_char(char c, int row, int col, char attr);


/*public API*/
void clear_screen(){
    int row;
    int col;
    for(row = 0; row < MAX_ROWS; row++){
        for(col = 0; col < MAX_COLS; col++){
            kprint_char(' ', row, col, 0);
        }
    }
    set_cursor_offset(get_screen_offset(0, 0));
}

void kprint_at(char *msg, int row, int col){
    int offset;
    for(int i = 0; msg[i] != 0; i++){
        offset = kprint_char(msg[i], row, col, 0);
        row = get_offset_row(offset); 
        col = get_offset_col(offset);
    }
}

void kprint(char *msg){
    kprint_at(msg, -1, -1);
}

void kprint_backspace(){
    int offset = get_cursor_offset() - 2;
    int row = get_offset_row(offset);
    int col = get_offset_col(offset);
    kprint_char(' ', row, col, 0);
    set_cursor_offset(offset);
}   

/***************************************************
 * *******helper function used by kprint_x**********
 * *************************************************/
/*write c at (row, col), set cursor to next charactor, and return the offset*/
int kprint_char(char c, int row, int col, char attr){
    unsigned char *video_mem = (unsigned char *)VIDEO_ADDRESS;
    if(attr == 0) attr = WHITH_ON_BLACK;

    if(col >= MAX_COLS || row >= MAX_ROWS){
        video_mem[2 * (MAX_ROWS * MAX_COLS) - 2] = 'E';
        video_mem[2 * (MAX_ROWS * MAX_COLS) - 1] = WHITH_ON_BLACK;
        return get_screen_offset(row, col);
    }
    //get offset of (row, col)
    int offset;
    if(row >= 0 && col >= 0) offset = get_screen_offset(row, col);
    else offset = get_cursor_offset();

    if(c == '\n'){
        // offset = get_screen_offset(row + 1, 0);
        //in case of row and col == 1, row will be change 
        row = get_offset_row(offset);
        offset  = get_screen_offset(row + 1, 0);
    }else{  
        video_mem[offset] = c;
        video_mem[offset + 1] = attr;
        offset += 2;
    }
    offset = handle_screen(offset);
    set_cursor_offset(offset);
    return offset;
}

//if offset beyond the screen, it will scroll dwon, else do nothing
int handle_screen(int offset){
    if(offset >= MAX_ROWS * MAX_COLS * 2){
        for(int row = 1; row < MAX_ROWS; row++){
            mem_copy((char *)(VIDEO_ADDRESS + get_screen_offset(row, 0)),
                    (char *)(VIDEO_ADDRESS + get_screen_offset(row - 1, 0)), MAX_COLS * 2);
        }
        char *last_line = (char *)get_screen_offset(MAX_ROWS - 1, 0) + VIDEO_ADDRESS;
        for(int i =0 ; i < MAX_COLS * 2; i++){
            last_line[i] = 0;
        }
        offset = get_screen_offset(MAX_ROWS - 1, 0);
    }
    return offset;  
}


int get_screen_offset(int row, int col){
    return (row * MAX_COLS + col) * 2;
}

int get_offset_row(int offset){
    return (offset/2) / MAX_COLS;
}

int get_offset_col(int offset){
    return (offset/2) % MAX_COLS;
}

int get_cursor_offset(){
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset  * 2; //keep in mind *2
}

void set_cursor_offset(int offset){
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    //14->high 8 bits
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    //15->low 8 bits
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

