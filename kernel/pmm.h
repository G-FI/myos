#ifndef PMM_H_
#define PMM_H_

#include <stdint.h>
#include "vmm.h"

void initialize_pmm();
void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void free_frame(page_t *page);


#endif //PMM_H_