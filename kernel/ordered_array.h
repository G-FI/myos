#ifndef ORDERED_ARRAY_H_
#define ORDERED_ARRAY_H_

/*generic ordered_array*/

#include <stdint.h>

typedef void* type_t; //ordered array store address of other data struct
typedef (*less_than_predicate)(void*, void*);

typedef struct{
    type_t *array;
    uint32_t size;
    uint32_t max;
    less_than_predicate less_than;
} ordered_array_t;

ordered_array_t place_ordered_array(void* start, uint32_t sz, less_than_predicate less_than);
void insert(type_t item, ordered_array_t *array);
void remove(uint32_t idx, ordered_array_t *array);
void increase_key(uint32_t idx, ordered_array_t *array);
void decrease_key(uint32_t idx, ordered_array_t *array);
uint32_t find(type_t item, ordered_array_t *array);

#endif //ORDERED_ARRAY_H_