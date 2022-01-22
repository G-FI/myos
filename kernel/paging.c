#include "paging.h"
#include "kheap.h"
#include "../libc/printf.h"

//bitmap for frames
uint32_t *frames;
uint32_t nframes;

//helper function
#define INDEX_FROM_BIT(fa) (fa/(8 * 4))
#define OFFSET_FROM_BIT(fa) (fa%(8 * 4))
static void set_frame(uint32_t fa);
static void clear_frame(uint32_t fa);
static uint32_t test_frame(uint32_t fa);
static uint32_t first_frame();
static void alloc_frame(page_t *page, int is_kernel, int is_writeable);
static void free_frame(page_t *page);

//public api
void initialize_paging(){
    uint32_t mem_end_page = 0x1000000;
    nframes = mem_end_page/0x1000;
    frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));

    //TODO memset

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
static void test_frame(uint32_t fa){
    uint32_t frame = fa/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    return (frams[idx] & (0x01 << off));
}

static uint32_t frist_frame(){
    for(uint32_t i = 0; i < INDEX_FROM_BIT(nframs); i++){
        if(frams[i]  != 0xFFFFFFFF){
            for(uint32_t j = 0; j < 32; j++){
                if((frams[i] & (0x01 << j)) == 0){
                    return i * 32 + j;
                }
            }
        }
    }
    return -1;
}

static void alloc_frame(page_t *page, int is_kernel, int is_writeable){
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

static void free_frame(page_t *page){
    if(page->fame == 0){
        return;
    }else{
        clear_frame(page->frame);
        page->frame = 0;
    }
}

