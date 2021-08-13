#include "pmem_manager.hpp"
#include <cstdio>
// could also use buddy system allocation
// bit map allocation

// Take all of memory, split it into "pages"/"chunks", then split into bytes
static pmem_map_container_t physical_memory_map[MAP_SIZE] = {0}; // Eventually MAX_MEM needs to be found, and this array dynamically allocated.

void pmem_init(){
    // 0x100000 = 1MB, so set first MB as allocated (low memory)
    // This is pretty useful for a lot of (low level) stuff and so lets not use it
    for (int i=0; i<(0x100000/(PAGE_SIZE*PMM_TABLE_SIZE));i++)
        physical_memory_map[i] = ~0;

    // Also allocate the space used by the program itself. (ideally given in that memory map from the bootloader)
    // Incorrect implementation but works for now...
    extern uint8_t _end;
    intptr_t end_addr = (intptr_t)&_end;
    int page_index = end_addr/PAGE_SIZE;
    int end_table = page_index / PMM_TABLE_SIZE; 
    int end_entry = page_index % PMM_TABLE_SIZE;
    for (int i=(0x100000/(PAGE_SIZE*PMM_TABLE_SIZE)); i<end_table;i++)
        physical_memory_map[i] = ~0;

    if (end_addr % PAGE_SIZE != 0)
        end_entry += 1; // Take up the extra page if the program uses part of a page
    // Set n bits, where n = bitmap_entry
    physical_memory_map[end_table] |= (1 << end_entry)-1;
    
    // Fill individual table
    // Load memory map here
    // TODO: .......

};

// Returns base address of a free page (no rules as to what kind of page)
uintptr_t pmem_alloc_page(){
    // Linear search for next free page
    // Future improvements:
    // * Keep index to the first free table (assume first n tables will be filled)
    // * Have a stack of pages before that index, that are free, and most likely fragmented. (very fast allocation)

    // Look through each table
    for(int i=0; i<MAP_SIZE;i++){
        // If the table isn't completely filled
        pmem_map_container_t table = physical_memory_map[i];
        if (table == ~(pmem_map_container_t)0)
            continue; // ~0 is all 1s, so this table is full
        
        for (int j=0; j<PMM_TABLE_SIZE; j++){
            bool page_used = table & (1<<j); // (table & (1<<j)) is the specific bit
            if(!page_used){ 
                physical_memory_map[i] |= 1 << j; // or it with our desired value
                return (i*PMM_TABLE_SIZE + j) * PAGE_SIZE;
            }
        }
        
    }
    return 0;
};

// Frees a given page
void pmem_free_page(uintptr_t base_address){
    int page_index = base_address/PAGE_SIZE;
    int bitmap_table = page_index / PMM_TABLE_SIZE; 
    int bitmap_entry = page_index % PMM_TABLE_SIZE;

    physical_memory_map[bitmap_table] &= ~(1 << bitmap_entry); // (x & !1) will clear that bit

};