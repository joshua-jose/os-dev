#pragma once
#include <cstdint>

#define PAGE_SIZE 4096 // Size of a single "chunk" that is defined by a single bit
#define MAX_MEM 1024*1024*16 // 16mb, this should be found from the BIOS!
#define PMM_TABLE_SIZE 64 // Size of uint8_t or whatever unit each array element is
#define MAP_SIZE MAX_MEM/(PAGE_SIZE*PMM_TABLE_SIZE)
typedef uint64_t pmem_map_container_t;

void pmem_init();

uintptr_t pmem_alloc_page();
void pmem_free_page(uintptr_t base_address);