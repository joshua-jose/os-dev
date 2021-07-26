#include <cstdint>

// could also use buddy system allocation
// bit map allocation

#define PAGE_SIZE 4096 // Size of a single "chunk" that is defined by a single bit
#define MAX_MEM 1024*1024*16 // 16mb, this should be found from the BIOS!
#define TABLE_SIZE 8 // Size of uint8_t or whatever unit each array element is
// Take all of memory, split it into "pages"/"chunks", then split into bytes
static uint8_t physical_memory_map[MAX_MEM/(PAGE_SIZE*TABLE_SIZE)] = {0}; // Eventually MAX_MEM needs to be found, and this array dynamically allocated.

void pmem_init(){
    // 0x100000 = 1MB, so set first MB as allocated (low memory)
    // This is pretty useful for a lot of (low level) stuff and so lets not use it
    for (int i=0; i<(0x100000/(PAGE_SIZE*TABLE_SIZE));i++)
        physical_memory_map[i] = ~0;

    // Load memory map here
    // TODO: .......

};

// Returns base address of a free page (no rules as to what kind of page)
uintptr_t pmem_alloc_page(){
    
};
// Frees a given page
void pmem_free_page(uintptr_t base_address);