#include "ordered_array.h"
#include "../libc/string.h"
#include "../libc/printf.h"

ordered_array_t place_ordered_array(void* start, uint32_t sz, less_than_predicate less_than){
    ordered_array_t ret;
    ret.array = (type_t*)(start);
    ret.size = 0;
    ret.max = sz;
    ret.less_than = less_than;
    memset(ret.array, 0, sz * sizeof(type_t));

    return ret;
}
void insert(type_t item, ordered_array_t *array){
    //ASSERT(array->size != array->max);
    int lo = 0, hi = array->size ;
    int idx = (lo + hi)/2;
    while(lo < hi){
        if(array->less_than(item, array->array[idx])){
            hi = idx;
            idx = (lo + hi)/2;
        }else{ //item >= array->arrary[idx]
            lo = idx + 1;
            idx = (lo + hi)/2;
        }
    }
    //idx will be the position to insert
    for(int i = array->size; i > idx; i--)
        array->array[i] = array->array[i-1];
    array->array[idx] = item;
    array->size +=1;
}
void remove(uint32_t idx, ordered_array_t *array){
    while(idx < array->size - 1){
        array->array[idx] = array->array[idx + 1];
        idx++;
    }
    array->size -= 1;
}
void increase_key(uint32_t idx, ordered_array_t *array){
    type_t tmp = array->array[idx];
    while((idx+1< array->size) && !array->less_than(tmp, array->array[idx + 1])){
        array->array[idx] = array->array[idx + 1];
        idx++;
    }
    //idx will be the right position
    array->array[idx] = tmp;
}
void decrease_key(uint32_t idx, ordered_array_t *array){
    type_t tmp = array->array[idx];
    while(idx > 0 && array->less_than(tmp, array->array[idx - 1])){
        array->array[idx] = array->array[idx - 1];
        idx--;
    }
    //idx will be the right position
    array->array[idx] = tmp;
}

uint32_t find(type_t item, ordered_array_t *array){
    for(uint32_t i = 0; i < array->size; i++)
        if(item == array->array[i])
            return i;
    
    PANIC("find");
}