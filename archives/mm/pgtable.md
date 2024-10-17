# pgtable

内核pgtable模块定义了页表的基本数据类型和结构，并提供了一组功能函数，来操作页表和设置页表项。

页表的内部结构和功能与arch强相关，ARM平台的页表结构可以参考ARMv8 MMU手册。

## Files

- /include/linux/pgtable.h
- /arch/arm64/include/asm/pgtable.h
- /mm/pgtable-generic.c

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

### Set VAL / BIT

```
set_pte
pte_clear
set_pte_bit
clear_pte_bit
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
pte_mkdevmap
```

### Check VAL / BIT

```
pte_none		// check an entry is empty
pte_bad			// check if the pages pmd refers to are inaccessable
pte_same		// check if two pte point to same PA
pte_dirty
pte_valid
pte_present		// check if entry / page is allocated and read/write/executable by present bit in entry
pte_young
pte_special
pte_write
pte_user
pte_user_exec
pte_cont
pte_devmap
pte_tagged	
```

### Entry Translation

```
pxx_page_paddr		// pa in pxx entry
pxx_page_vaddr		// va of the pa in pxx entry
__pxx_to_phys		// pa in pxx entry
__phys_to_pxx_val	// convert pa to pxx entry which contain pa
pxx_val			// pxx.pxx, real pxx entry value
pxx_pfn / pfn_pxx	// pa in pxx to pfn
pxx_page		// pa in pxx to page struct convert
pxx_pXX			// pxx to pXX convert
pxx_pgtable(pXX)	// va of the pa in pxx entry (pxx table pointer)
pxx_index(addr)		// parse pxx entry from vaddr, return pxx index in its table
pxx_offset(pXX, addr)	// parse pxx entry from vaddr, return pxx entry va pointer
```

### Entry Fixmap

```
pxx_set_fixmap(addr)			// create fix address mapping for p-addr, return pxx table va pointer
pxx_set_fixmap_offset(pXX, addr)	// call pxx_set_fixmap with v-addr (va backend pxx table va pointer)
```

### Entry Allocation

```
pxx_alloc(mm, pXX, addr)
pxx_free(x)
```

## Module Test

[test-pgtable](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-pgtable)