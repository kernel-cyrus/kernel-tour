# memblock

memblock是在early boot阶段使用的memory allocator。

memblock会从fdt获取整个物理内存的layout，并初始化内存及占用状态，管理early boot阶段的物理内存的分配和使用。

memblock最主要的功能是对外提供了 `memblock_alloc` 接口，供early boot阶段的模块申请动态内存使用。

memblock的生命周期从setup_arch开始，到mm_init结束。内存管理权会从memblock移交给buddysystem，这时如果继续使用 `memblock_alloc` 申请内存，会从buddy system分配，同时报出WARNING。

memblock除了提供出内存分配接口外，还提供了内存块遍历，及debugfs的dump接口。

实现上，memblock内部主要有两个memory(type)数组，管理内存的分配：

- memory 系统中的全部物理内存
- reserved 被占用的物理内存（allocated or reserved）

整个memblock的管理及功能接口主要就是围绕这两个数组实现的。

## Files

- mm/memblock.c

## Structures

`struct memblock`

memblock全局对象。

`struct memblock_type`

memblock中主要包括两个type：memory，reserved（如上所述）。

每个type下包含一个regions串联起来的数组，每个region就是一个内存区域块。

`struct memblock_region`

一个内存block(region)，保存了这个region的start（PA）、size、flags。

## Functions

Android平台没有使能NUMA，所以这里没有包含相关函数

### Memblock Management

```
memblock_add			// Add range of phys memory to memory list
memblock_remove			// Remove range of phys memory from memory list
memblock_reserve		// Add range of phys memory to reserved list
memblock_trim_memory		// Align each memory region in memory list
memblock_overlaps_region	// Check if region has overlaps with region in type mem list
memblock_mark_hotplug		// mark region as hotplug mem
memblock_mark_mirror		// mark region as mirror mem
memblock_mark_nomap		// mark region as nomap (not be added to linear mapping)
```

### Loop Iterators

```
for_each_mem_region		// Loopper for memory list
for_each_reserved_mem_region	// Loopper for reserved list
for_each_free_mem_range		// Looper for free areas (memory and !reserved)
```

### Memory Alloc & Free

#### Boot time mem alloc return VA

```
memblock_alloc			// alloc mem block and memset to 0
memblock_alloc_raw		// alloc mem block without memset
memblock_alloc_from		// alloc mem block from a base addr
memblock_alloc_low		// alloc mem block from low addr (< 0xffffffff)
memblock_free			// free the mem block
```

#### Boot time mem alloc return PA

```
memblock_phys_alloc		// alloc and return "PA"
memblock_phys_alloc_range	// alloc and return "PA"
memblock_phys_free		// free by "PA"
```

### Get & Set

```
memblock_set_current_limit			// Set alloc max limit addr
memblock_get_current_limit			// Get alloc max limit addr
memblock_phys_mem_size(void)			// Total Phys Mem Size
memblock_reserved_size(void)			// Reserved Mem Size (Used or preserved Reserved)
memblock_start_of_DRAM(void)			// DDR start PA
memblock_end_of_DRAM(void)			// DDR end PA
memblock_is_memory(addr)			// Is paddr in memory
memblock_is_map_memory(addr)			// Is paddr mapped
memblock_is_region_memory(base, size)		// Is region in memory
memblock_is_reserved(addr)			// Is paddr reserved
memblock_is_region_reserved(base, size)		// is region reserved
memblock_region_memory_base_pfn(region)		// 返回region的起始PFN
memblock_region_memory_end_pfn(region)		// 返回region的结尾PFN
memblock_region_reserved_base_pfn(region)	// 返回region的起始PFN
memblock_region_reserved_end_pfn(region)	// 返回region的结尾PFN
memblock_set_bottom_up				// 设置alloc时，从顶开始分配还是从底开始分配
memblock_bottom_up				// 返回alloc时，从顶开始分配还是从底开始分配
```

### Other Functions

```
early_memtest		// Test all free mem area
memblock_dump_all	// Dump memblock layout to console
memblock_free_all	// Release all free pages to buddy system
```

## Usage Test

**Enable memblock debug print**

cmdline传入"memblock=debug"，会在log中打印将所有memblock操作

**Enable bootmem test**

cmdline传入“memtest”，会在启动时遍历所有memblock free area，做读写测试

**Dump memblock layout from sysfs**

```
/sys/kernel/debug/memblock/memory	// Dump all memory regions
/sys/kernel/debug/memblock/reserved	// Dump all reserved regions
```

## Module Test

[test-memblock](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-memblock)