#include "kheap.h"
#include "../libc/function.h"
#include "pmm.h" //for FRAMESZ

heap_t *kheap;

extern uint32_t end;

uint32_t placement_address = (uint32_t*)&end;
extern pagetable_dir_t* kernel_dir;

#define PGROUNDUP(sz) (((sz) + FRAMESZ-1) & ~(FRAMESZ-1))
#define PGROUNDDOWN(pa) ((sz) & ~(FRAMESZ-1))
#define FOOTER(header) ((footer_t*)((uint32_t)header + header->size - sizeof(footer_t)))
#define PREHEADER(h) (((footer_t*)((uint32_t)h - sizeof(footer_t)))->header)
#define NEXTHEADER(header) ((header_t*)((uint32_t)header + header->size))

//pre-declaration
static uint32_t kheap_aux(uint32_t sz, int aligned, uint32_t *pa);

uint32_t kmalloc(uint32_t sz){
    return kheap_aux(sz, 0, 0);
}
uint32_t kmalloc_a(uint32_t sz){
    return kheap_aux(sz, 1, 0);
}
uint32_t  kmalloc_ap(uint32_t sz, uint32_t *pa){
    return kheap_aux(sz, 1, pa);
}
uint32_t kmalloc_p(uint32_t sz, uint32_t *pa){
    return kheap_aux(sz, 0, pa);
}

//if aligned, the start address of allocated should be aligned
static uint32_t kheap_aux(uint32_t sz, int aligned, uint32_t *pa){
    if(!kheap){
        if(aligned){
            if(placement_address & 0xFFF){
                placement_address += FRAMESZ;
                placement_address &= 0xFFFFF000;
            }
        }

        if(pa){
            *pa = placement_address;
        }
        uint32_t start_addr = placement_address;
        placement_address += sz;
        return start_addr;
    }else{
    //heap can be used
        uint32_t ret = (uint32_t)alloc(sz, aligned, kheap);
        if(pa){
            pte_t *pte = get_pte(pa, 0, kernel_dir);
            *pa = pte->frame << 12 + (ret & 0xFFF);
        }
        return ret;
    }
}


//kheap helper function
int find_smallest_hole(uint32_t sz, int aligned, heap_t *heap);
void expand(uint32_t newsz, heap_t *heap);
void extract(uint32_t newsz, heap_t *heap);
void create_hole(uint32_t start_addr, uint32_t end_addr, heap_t *heap);
void contract(header_t *header, heap_t *heap);
int less_than(void *x, void *y){
    return ((header_t*)x)->size < ((header_t*)y)->size;
}

heap_t* create_heap(uint32_t start, uint32_t end, uint32_t max, int supervisor, int writable){
    
    //page has already enabled, and heap space has been mapped in pagetable 
    //heap_t in indentify map address
    heap_t *heap = (heap_t*)kmalloc_a(sizeof(heap_t));
    
    //heap->headers_ptr and heap space was palced in address
    //right after the kernel data, but the virtual address begin at 
    //address start 
    heap->headers_ptr = place_ordered_array((void*)start, KHEAP_INDEX_SIZE, less_than);
    
    //kheap space start after heap->array, start = 0xC0008000
    start += KHEAP_INDEX_SIZE * sizeof(type_t);

    //kheap 4kib align
    if(start & 0xFFF != 0){
        start += 0x1000;
        start &= 0xFFF;
    }

    heap->start_addr = start;
    heap->end_addr = end;
    heap->max_addr = max;
    heap->supervisor = supervisor;
    heap->writable = writable;
    
    //now our heap has one hole
    header_t *header = (header_t*)start;
    header->size = end - start;
    header->magic = MAGIC;
    header->is_hole = 1;
    //set the end of heap space be a footer
    footer_t *footer = (footer_t*)(start + header->size - sizeof(footer_t)); 
    footer->header = header;
    footer->magic = MAGIC;
    
    insert(header, &heap->headers_ptr);
    return heap;
}

void* alloc(uint32_t sz, int aligned, heap_t *heap){
    uint32_t idx = find_smallest_hole(sz, aligned, heap);
    //没有找到合适大小的hole
    if(idx == (uint32_t)-1){ 
        uint32_t newsz = heap->end_addr - heap->start_addr + sz + sizeof(header_t) + sizeof(footer_t);
        expand(newsz, heap);
        return alloc(sz, aligned, heap);
    }
    //found the fitted hole
    header_t *header = (header_t*)heap->headers_ptr.array[idx];
    footer_t *footer = FOOTER(header);
    uint32_t oldsz = header->size;
    //既然找到了能用的hole，一定要从可使用headers中先删除
    idx = find(header, &kheap->headers_ptr);
    remove(idx, &kheap->headers_ptr);

    if(aligned){
        uint32_t offset = FRAMESZ - (uint32_t)header%FRAMESZ - sizeof(header_t);  //offset = distance between header to new_header
        header_t *new_header = (header_t*)((uint32_t)header + offset);

        if(offset > sizeof(header_t) + sizeof(footer_t)){
            //header到对齐点之间有空间可以用，足够分成一个单独的hole
            create_hole((uint32_t)header, (uint32_t)new_header, kheap);
        
        }else{
            //不够，就需要合并到前一个hole或者block（不管它是不是空闲，因为要保证堆区是全部连接的）
            header_t *pre_header = PREHEADER(header);
            //add_space = offset
            pre_header->size += offset;
            //size of header changed, so we need set a new footer
            footer_t *pre_footer = FOOTER(pre_header);
            pre_footer->magic = MAGIC;
            pre_footer->header = pre_footer;
            //remove origin hole, create new hole insert to ordered_array and increase the key of previous block
            // idx = find(pre_header, &kheap->headers_ptr);
            // increase_key(idx, &kheap->headers_ptr);
            //如果左边是一个hole，就需要调整有序数组
            if(pre_header->is_hole){
                idx = find(pre_header, &kheap->headers_ptr);
                increase_key(idx, kheap->headers_ptr)
            }
            
        }
        //adjust remaining space 
        new_header->magic = MAGIC;
        new_header->size = oldsz - offset;
        header = new_header;
        //left previous footer->header point to the new header
        footer->header = header;
    }
    //现在满足，分配成功，并且左边已经调整好，[header, footer), 剩下的就是右边，是否需要再拆分
    
    //check if split a new hole on the right
    if(sz + 2*(sizeof(header_t) + sizeof(footer_t)) < header->size){
        uint32_t start = (uint32_t)header + sizeof(header_t) + sz +sizeof(footer_t);
        uint32_t end = (uint32_t)header + header->size;
        create_hole(start, end, kheap);
        header->size = header->size - (end - start);
        //size of header changed, so we need to set a new footer
        footer = FOOTER(header);
        footer->magic = MAGIC;
        footer->header = header;
    }
    //应该
    //insert(header, &heap->headers_ptr);

    header->is_hole = 0;
    return (void*) header + sizeof(header_t);
}

void* free(void *p, heap_t *heap){
    header_t *header = (header_t*)(p - sizeof(header_t));
    ASSERT(header->magic =  MAGIC);
    contract(header, heap);
}

//return the index of smallest  hole fitted
/*if !aligned, sz + header + footer < block size
  if aligned, offset:from header to align point,  offset + sz + footer < blocksize*/
int find_smallest_hole(uint32_t sz, int aligned, heap_t *heap){
    header_t *header;
    uint32_t i;
    for(i = 0; i < heap->headers_ptr.size; i++){
        header = (header_t*)(heap->headers_ptr.array[i]);
        if(sz + sizeof(header_t) + sizeof(footer_t) > header ->size)
            continue;
        //大小够用
        if(aligned){
            int offset = FRAMESZ - ((uint32_t)header) & 0xFFF;
            if(offset + sz + sizeof(footer_t) < header->size){
                break;
            }
        }else{
            break;
        }
    }
    if(i == heap->headers_ptr.size)
        return -1;
    return i;
}


/**for expand assume heap->end_addr always page aligned\
  and expand must map page, extract must unmap page,
  they are not adjust header and footer, you need to 
  do it outside of them**/
//for example: set new allocated block as a hole, the 
//use contract to unify
void expand(uint32_t newsz, heap_t *heap){
    //assert(heap->end_addr< heap->start_addr + newsz < heap->max_addr)
    uint32_t new_end = PGROUNDUP(newsz+heap->start_addr);
    uint32_t va, old_end;
    va = old_end = heap->end_addr;
    //map from old_end to new_end
    while(va < new_end){
        alloc_frame(get_pte(va, 1, kernel_dir), heap->supervisor? 1:0, heap->writable? 1:0);
        va += FRAMESZ;
    }
    heap->end_addr = new_end;
    create_hole(old_end, new_end, kheap);
    contract((header_t*)old_end, heap);
}

//TODO: remove from ordered_array, 
void extract(uint32_t newsz, heap_t *heap){
    //assert(heap->start_addr< heap->staexrt_addr+newsz < heap->end_addr)
    if(newsz < KHEAP_MIN_SIZE)
        return;
    
    uint32_t new_end = PGROUNDUP(newsz + heap->start_addr); 
    uint32_t va = heap->end_addr - FRAMESZ;
    while(va >= new_end){
        free_frame(get_pte(va, 0, kernel_dir));
        va -= FRAMESZ;
    }

    heap->end_addr = new_end;
}

//hole[start_addr, end_addr) assume hole_size(end_addr - start_addr) >= sizeof(header_t) + sizeof(footer_t) 
//create_hole ensure add the new hole to header_pointer_array
void create_hole(uint32_t start_addr, uint32_t end_addr, heap_t *heap){
    header_t *header = (header_t*)start_addr;
    header->magic = MAGIC;
    header->is_hole = 1;
    header->size = end_addr - start_addr;
    footer_t *footer = (footer_t*)(end_addr - sizeof(footer_t));
    footer->magic = MAGIC;
    footer->header = header;
    insert(header, heap);
}

//unify left or right, if it is a hole 
//传入的header：还没有被插入回ordered_array
void contract(header_t *h, heap_t *heap){

    header_t *header = h;
    footer_t *footer = FOOTER(header);

    //boundary check
    if((uint32_t)footer + sizeof(footer_t) < heap->end_addr){
        header = NEXTHEADER(h);
        footer = FOOTER(header); //footer of next hole
        ASSERT(header->magic == MAGIC);
        if(header->is_hole){
            //unify right
            h->size += header->size;
            footer->header = h;
            //remove next block
            uint32_t idx = find(header, &heap->headers_ptr);
            remove(idx, &heap->headers_ptr);
        }
        header = h;
        footer = FOOTER(header);
    }


    if((uint32_t)header > heap->start_addr){
        header_t *header = PREHEADER(h);
        footer_t *footer = FOOTER(h);
        ASSERT(header->magic == MAGIC);
        if(header->is_hole){
            //unify left
            header->size += h->size;
            footer->header = header;

            //如果左边可以合并，因为当前hole还没有插入到数组中，所以直接给左边的hole增加大小，然后increase_key
            uint32_t idx = find(header, &heap->headers_ptr);
            increase_key(idx, &heap->headers_ptr);
            return;
        }
    }

    //对于只合并右边的情况
    uint32_t idx = find(h, &heap->headers_ptr);
    increase_key(idx, &heap->headers_ptr);
    
}