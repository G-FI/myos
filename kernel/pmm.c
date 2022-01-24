#include "pmm.h"
#include "vmm.h"
#include "kheap.h"
#include "../libc/string.h"
#include "../libc/printf.h"

//bitmap for frames
char *frames;
uint32_t nframes;

//helper function
#define INDEX_FROM_BIT(fa) (fa/ 8)
#define OFFSET_FROM_BIT(fa) (fa% 8)
static void set_frame(uint32_t fa);
static void clear_frame(uint32_t fa);
static uint32_t test_frame(uint32_t fa);
static uint32_t first_frame();


//public API
void alloc_frame(pte_t *pte, int is_kernel, int is_writeable){
    //return if frame was already allocated
    if(pte->frame){
        PANIC("remap");
    }else{
        uint32_t idx = first_frame();
        if(idx == (uint32_t)-1){
            PANIC("alloc_frame" );
        }
        set_frame(idx * 0x1000);
        pte->present = 1; // Mark it as present.
        pte->rw = (is_writeable)?1:0; // Should the page be writeable?
        pte->user = (is_kernel)?0:1; // Should the page be user-mode?
        pte->frame = idx;
    }
}

void free_frame(pte_t *pte){
    if(pte->frame == 0){
        PANIC("free_frame");
    }else{
        clear_frame(pte->frame);
        pte->frame = 0;
    }
}


static void set_frame(uint32_t fa){
    uint32_t frame = fa/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x01 << off);
}
static void clear_frame(uint32_t fa){
    uint32_t frame = fa/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x01 << off);
}
static uint32_t test_frame(uint32_t fa){
    uint32_t frame = fa/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x01 << off));
}

static uint32_t first_frame(){
    for(uint32_t i = 0; i < INDEX_FROM_BIT(nframes); i++){
        if(frames[i]  != 0xFFFFFFFF){
            for(uint32_t j = 0; j < 8; j++){
                if((frames[i] & (0x01 << j)) == 0){
                    return i * 8 + j;
                }
            }
        }
    }
    return -1;
}


