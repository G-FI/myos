#include "pmm.h"
#include "vmm.h"
#include "../libc/string.h"
#include "../libc/printf.h"

//Macro for pmm
#define FRAMESZ 0x1000
#define MEM_END 0x1000000   //the size of physical address 16Mib

//defined in linker.ld
extern uint32_t end;

//bitmap for frames
uint32_t *frames;
uint32_t nframes;
uint32_t placement_address = (uint32_t)&end;

//helper function
#define INDEX_FROM_BIT(fa) (fa/(8 * 4))
#define OFFSET_FROM_BIT(fa) (fa%(8 * 4))
static void set_frame(uint32_t fa);
static void clear_frame(uint32_t fa);
static uint32_t test_frame(uint32_t fa);
static uint32_t first_frame();

//public API
void initialize_pmm(){
    nframes = MEM_END/FRAMESZ;
    frames = (uint32_t*)placement_address;
    placement_address += (nframes/8);
    memset(frames, 0, nframes/8); //nframes is number of bits
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable){
    //return if frame was already allocated
    if(page->frame){
        return;
    }else{
        uint32_t idx = first_frame();
        if(idx == (uint32_t)-1){
            PANIC("alloc_frame" );
        }
        set_frame(idx * 0x1000);
        page->present = 1; // Mark it as present.
        page->rw = (is_writeable)?1:0; // Should the page be writeable?
        page->user = (is_kernel)?0:1; // Should the page be user-mode?
        page->frame = idx;
    }
}

void free_frame(page_t *page){
    if(page->frame == 0){
        return;
    }else{
        clear_frame(page->frame);
        page->frame = 0;
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
            for(uint32_t j = 0; j < 32; j++){
                if((frames[i] & (0x01 << j)) == 0){
                    return i * 32 + j;
                }
            }
        }
    }
    return -1;
}


