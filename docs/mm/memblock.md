# memblock

memblock是在early boot阶段使用的memory allocator。

memblock会从fdt获取整个物理内存的layout，并初始化内存及占用状态，管理early boot阶段的物理内存的分配和使用。

memblock最主要的功能是对外提供了 `memblock_alloc` 接口，供early boot阶段的模块申请动态内存使用。

memblock的生命周期从setup_arch开始，到mm_init结束。内存管理权会从memblock移交给buddysystem，`memblock_alloc` 也就不再能继续使用了。

memblock除了提供出内存分配接口外，还提供了内存块遍历，及debugfs的dump接口。

## Files

- mm/memblock.c

## Structures

`struct memblock`

memblock全局对象

`struct memblock_region`


## Functions

### Status

memblock_phys_mem_size(void)
memblock_reserved_size(void)
memblock_start_of_DRAM(void)
memblock_end_of_DRAM(void)
memblock_enforce_memory_limit(memory_limit)
memblock_cap_memory_range(base, size)
memblock_mem_limit_remove_map(limit)
memblock_is_memory(addr)
memblock_is_map_memory(addr)
memblock_is_region_memory(base, size)
memblock_is_reserved(addr)
memblock_is_region_reserved(base, size)


### Iterator

`for_each_mem_region`, `for_each_reserved_mem_region`

### Convertion

`memblock_region_memory_base_pfn`
`memblock_region_memory_end_pfn`
`memblock_region_reserved_base_pfn`
`memblock_region_reserved_end_pfn`

返回region的起始、结尾PFN

### Other

`memblock_dump_all`

`early_memtest`