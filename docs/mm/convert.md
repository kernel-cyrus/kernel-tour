# convert

内核中一个内存页对应了几个基本概念：

- 虚拟地址（virt）：页表转化前的虚拟地址
- 物理地址（phys）：页表转化后的物理地址
- page frame（pfn）：每个物理页(4k page)，有一个ID，这个ID就是pfn
- page struct（page）：每个物理页(4k page)，对应了一个page struct数据结构，内核用来管理和描述这个物理页的状态
- page table entry：这个物理页对应的各级页表Entry（pgd pud pmd pte）

这些数据的地址间，可以互相转化和访问。

**Linear Mapping**

整个Kernel的VA地址空间的总体划分 `memory.h`，使用ptdump（address_markers）可以把这个layout dump出来。

`PAGE_OFFSET` ~ `PAGE_END` 这段VA区间，对全部物理内存做了完整的一一映射，这个映射叫做Linear Mapping。

Linear Mapping的VA与PA间，可以进行线性转换（计算时，只需要计算PA在物理内存的内部偏移址，在PAGE_OFFSET基础上做同样偏移，就得到了VA地址）

Linear Mapping，使用了一个pgd：pgd[0]，在 `paging_init` `map_mem` 过程中完成。

利用Linear Mapping这个映射，我们就可以在内核中，去操作物理内存，或是找到物理地址的句柄指针。

**Page Table Entry**

这里的转换，不包括页表Entry的访问，访问页表Entry，见 [mm/pgtable]([pgtable](/mm/pgtable))

## Files

```
- /arch/arm64/include/asm/memory.h
- /arch/arm64/mm/mmu.c
- /include/asm-generic/memory_model.h
```

## Functions

### virt <-> phys - page - pfn（Linar Mapping）

`virt_to_phys` `phys_to_virt` `__virt_to_phys` `__phys_to_virt` `__va` `__pa`

虚拟地址与物理地址通过Linear Mapping转换。

这几个接口功能相同，支持的VA地址范围包括：

- Linear Mapping的VA范围（PAGE_OFFSET ~ PAGE_END）
- Kernel Image的VA范围（KERNEL_START ~ KERNEL_END）

在开启CONFIG_DEBUG_VIRTUAL的情况下，传入Kernel Image符号VA会导致WARNING，所以不建议使用这些接口转化内核符号地址。

`virt_addr_valid` `__is_lm_address`

要检查一个vaddr是否属于Linear Mapping的范围，可以使用这类接口。

### virt <-> phys - page - pfn（Kernel Symbol）

`__kimg_to_phys` `__phys_to_kimg` `__pa_symbol`

Kernel镜像内符号的虚拟地址与物理地址转换（Buildtime VA <=> Runtime PA）

使用这个接口的原因，是Kernel镜像内的符号虚拟地址不在Linear Mapping的VA范围内。

内核符号建议使用这类接口转换。

### virt <- phys -> page - pfn

`virt_to_page` `page_to_virt`

虚拟地址与其所映射的物理页对应的page struct之间转换。

### virt <- phys - page -> pfn

`virt_to_pfn`

虚拟地址与其所映射的物理页的PFN转换。

### virt - phys <-> page - pfn

`page_to_phys` `phys_to_page`

物理地址与其对应的物理页的page struct之间转换。

### virt - phys <- page -> pfn

`__phys_to_pfn` `__pfn_to_phys`

物理地址与其对应的物理页的PFN之间转换。

### virt - phys - page <-> pfn

`page_to_pfn` `pfn_to_page`

物理页对应的page struct与PFN转换。

### to pgd pud pmd pte

页表的访问和修改，见 [mm/pgtable]([pgtable](/mm/pgtable))

### symbols

`__pa_symbol` `__phys_addr_symbol` `sym_to_pfn`

### other

`pfn_to_kaddr`

## Usage Test

CONFIG_DEBUG_VIRTUAL

CONFIG_DEBUG_VM

## Module Test

[test-mmconv](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-mmconv)

## Reference

https://www.cnblogs.com/liuhailong0112/p/14465697.html