# pgtable

内核pgtable模块定义了页表的基本数据类型和结构，并提供了一组功能函数，来操作页表和设置页表项。

页表的内部结构和功能与arch强相关，ARM平台的页表结构可以参考ARMv8 MMU手册。

## Files

- /include/linux/pgtable.h
- /arch/arm64/include/asm/pgtable.h
- /mm/pgtable-generic.c

与大多数arch相关的内核机制实现一样，整个pgtable的功能实现分为两层：
- 第一层是在`linux/pgtable.h`中封装出了不同平台统一的页表操作接口。
- 第二层是在`asm/pgtable.h`中实现了arch操作的具体功能。

可以看到第一层所封装的接口，都是调用第二层中的接口来完成具体功能操作的。

这样，pgtable在不同平台上，对外封装了统一接口，操作页表只需要引用`linux/pgtable.h`即可。

## Structures

### Page Table

Kernel包括以下几个Page Table，共不同启动阶段切换使用。

其中，启动阶段页表主要用于MMU使能前后的PA到VA跳转，因为跳转时需要有一个页表来做VA=PA的一一映射。

这些数据结构中保存了不同启动阶段使用的一级页表内容，而二三级页表保存在物理内存尾部的？MB空间内。

这些数据结构的基地址，就是设置到TTBRn_EL1页表寄存器的页表地址。

```
extern pgd_t init_pg_dir[PTRS_PER_PGD];		// 启动阶段，高地址页表（TTBR1_EL1）
extern pgd_t init_pg_end[];
extern pgd_t swapper_pg_dir[PTRS_PER_PGD];	// 最终内核空间使用的一级页表（TTBR1_EL1）
extern pgd_t idmap_pg_dir[PTRS_PER_PGD];	// 启动阶段，低地址页表（TTBR0_EL1）
extern pgd_t idmap_pg_end[];
extern pgd_t tramp_pg_dir[PTRS_PER_PGD];
extern pgd_t reserved_pg_dir[PTRS_PER_PGD];
```

### Page Table Entry

Page Table，由Page Table Entry组成。

每个Entry指向下一级Page Table的基地址，或者直接映射一块大的Memory Block。

内核中定义了：`pgd_t`, `pud_t`, `pmd_t`, `pte_t` 四种数据类型，分别表示四级页表的Entry结构。

Entry的大小和内部结构由arch定义，以ARMv8为例，Kernel的Page Table Entry表示的是MMU中定义的Page Table Descriptor的结构。

## Functions

### Common

```
set_p**		// set value to entry
p**_none	// check an entry is empty
p**_clear	// clear an entry
p**_bad		// check if the pages pmd refers to are inaccessable
p**_present	// check if entry / page is allocated and read/write/executable by present bit in entry
pte_same	// check if two pte point to same PA
pmd_large	// check if pmd refers to a large memory block
```

### Set PTE、PMD

```
clear_pte_bit
set_pte_bit
clear_pmd_bit
set_pmd_bit
pte_mkwrite
pte_mkclean
pte_mkdirty
pte_wrprotect
pte_mkold
pte_mkyoung
pte_mkspecial
pte_mkcont
pte_mknoncont
pte_mkpresent
pmd_mkcont
pte_mkdevmap
set_pte
```

### Check PTE、PMD

```
pte_present
pte_young
pte_special
pte_write
pte_user
pte_user_exec
pte_cont
pte_devmap
pte_tagged	
```

### Table Operations

页表操作函数，主要提供了以下几类数据间的提取和转化，括号中是这类功能函数可能包括的关键字。

```
- p** entry		(_val)
- p** entry va		(_offset)
- p*** va in p** entry	(_page, _to_page, _page_vaddr)
- p*** pa in p** entry	(_page_paddr, to_phys)
```

Table Entry中的PA=>VA是怎么完成的？

整个Kernel的VA空间（划分见 `memory.h`），`PAGE_OFFSET` ~ `PAGE_END` 这段VA区间，对物理内存做了完整的一一映射，通过 `__va` 函数，可以将任何PA转换为VA。计算时，只需要计算PA在物理内存的内部偏移址，在PAGE_OFFSET基础上做同样偏移，就得到了VA地址。

这个一一映射（direct mapping），使用了一个pgd：pgd[0]，在 `paging_init` / `map_mem` 过程中完成。

```
p**_val			// pte entry content
p**_page		// return p***'s in pte entry
p**_page_paddr		// next level pa in pte entry
p**_index(addr)		// get p** index from addr
p**_offset(addr)	// get p** va addr from addr
p**_addr_end(addr)	// get va of p** mem range end
mk_pte(p,prot)
pte_to_pgoff(pte)
pgoff_to_pte(offset)
```

### Entry Allocation

```
pgd_alloc(mm)
pgd_free( pgd)
pud_alloc(mm, pgd, addr)
pud_free(x)
pmd_alloc(mm, pud, addr)
pmd_free(x )
pte_alloc_map(mm, pmd, addr)
pte_alloc_kernel(mm, pmd,addr)
pte_free( pte)
pte_free_kernel(pte)
clear_page_range(mmu,start,end)
```

### Entry Translate

```
pte_pfn
pfn_pte
p**_p**		// pgd <=> pud <=> pmd <=> pte
```

## Module Test

[test-pgtable](https://github.com/kernel-cyrus/test-modules/tree/master/test-pgtable)