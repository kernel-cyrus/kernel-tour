# mmu

本文关于页表建立过程及相关函数。

mmu文件内的相关接口，是pgtable接口的更上层封装。pgtable接口用于定义及页表entry的底层操作。mmu接口则关注页表建立过程。

## Files

```
- /arch/arm64/mm/mmu.c			# 页表操作高级接口，页表创建过程
- /arch/arm64/kvm/hyp/pgtable.c		# 页表底层定义及操作接口
```

## Variables

`pgdp` `(p4dp)` `pudp` `pmdp` `ptep`

pxx table中的某个entry的pointer。指针通过fixmap映射而来，通过++操作，可以遍历fixmap映射的所有entry。

在没有p4d的情况下，p4dp = pgdp

## Functions

### Init

`paging_init`

页表创建函数，负责最终内核页表的建立。会将Kernel Image的各个Section，映射到其编译时指定VA地址，为其创建出对应的页表项。还会将已映射的内存重新map迁移到新的页表。最后将TTBR1切换到swapper_pg_dir（最终内核页表）

init_mm.pgd保存了swapper_pg_dir的指针，mmu的页表操作接口利用这个“根”指针来访问和操作页表。

`map_kernel` `map_kernel_segment`

负责将kernel的各个段映射到其buildtime的VA上，映射后kernel image的这些符号的va地址就可以访问和正常跳转了。

`map_mem`

从memblock获取整个物理内存layout，为全部物理内存建立Linear Mapping。

### Fixmap

`pxx_set_fixmap` `pxx_clear_fixmap`

将pxx table(4k)，映射到fixmap的虚拟地址，得到这个4k table的访问指针，或者清除这个映射。

pgd、pud、pmd、pte的四类table，在fixmap中，都有一个IDX可以用来做映射，每类table有一个IDX。（四个slot）

fixmap可以利用这四个IDX，不断将4k固定虚拟地址空间，重映射到不同的pxx table地址上，完成对任意table的访问和操作。（简单理解，就是向操作哪个table，就用对应的IDX，将VA映射到这个table的物理内存上）

`pxx_set_fixmap_offset`

通过vaddr来返回其对应的pxx entry指针。这个指针可以访问后面4K的连续物理内存。

这个接口会利用传入的PXX entry，找到其中保存的pxx table物理地址。然后根据vaddr计算出对应的是哪个pxx entry，然后为这个entry开始的后面4K物理内存建立fixmap映射，并返回这个entry的访问指针。

`fixmap_pxx`

返回已经映射pxx entry pointer。

### Create

`create_pgd_mapping` 

供外部使用的页表创建接口。

不允许外部使用此接口创建内核空间页表（BUG_ON use init_mm）

`__create_pgd_mapping`

为一块VA区域到一块连续物理内存建立映射。会根据传入的地址范围，loop范围对应的每个pgd entry，调用下一级页表创建接口，逐级创建出页表。

`alloc_init_pud`，`alloc_init_cont_pmd` ( `init_pmd` )，`alloc_init_cont_pte` (`init_pte` )

调用传入的alloc接口，alloc出pxx table（如果传入的PXX entry尚未初始化过），并根据传入的VA范围，逐一loop范围内对应的每一个 entry，调用下一级页表创建接口，为每一个entry继续创建下级table。

`__pxx_populate`

相当于set pxx entry。

### Alloc

页表创建时使用的不同alloc回调，从物理内存中alloc一个4k的pxx table，并返回物理地址。

`pgd_pgtable_alloc` 

外部调用 `create_pgd_mapping` 创建页表时使用的alloc。使用get free page，返回一个物理页。

`early_pgtable_alloc`

`map_kernel` `map_mem` 时使用的alloc，通过memblock alloc，申请一块4K物理内存并返回物理地址。

### Huge

`pud_set_huge` `pmd_set_huge`

将pxx entry设置为大页（一个entry直接映射以整块物理内存，无需再建立下级页表）

## Tests

1. create_pgd_mapping 创建页表

2. modify PROT_DEFAULT 修改默认页属性（cache policy）