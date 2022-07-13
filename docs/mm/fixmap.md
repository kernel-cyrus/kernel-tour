# fixmap

fixmap是一种VA到PA的映射机制，可以将一块预定义的固定4K VA地址范围动态映射到某一块物理内存。

fixmap在使用前，需要预定的一个访问IDX，fixmap会为每个IDX预留一块4K的VA区域，用于其之后映射到所需的PA区域。

fixmap在映射各个IDX区域到PA时，使用pgd[507]及编译时被预留在.bss段的bm_pmd, bm_pud, bm_pte作为页表。

# Files

```
- include/asm-generic/fixmap.h		# Fixmap interfaces declearation
- arch/arm64/include/asm/memory.h	# Fixmap IDX declearation
- arch/arm64/mm/mmu.c			# Fixmap mapping implementation
```

# Structures

`enum fixed_address`

定义fixmap访问ID，fixmap会为每个在这里定义的ID预留出一块固定的4K VA地址区间。

# Functions

## Interface

`fix_to_virt`

将IDX转化为对应的VA起始地址

`virt_to_fix`

将VA地址转化为IDX

`set_fixmap`

为IDX映射物理内存，VA和PA地址都会做4K对齐，完成后无返回值

`set_fixmap_offset`

同 set_fixmap，完成后返回IDX对应的VA Pointer

`set_fixmap_io`, `set_fixmap_offset_io`

同 set_fixmap, set_fixmap_offset, 将PA映射为device memory

`set_fixmap_nocache`, `set_fixmap_offset_nocache`

同 set_fixmap, set_fixmap_offset, 将PA映射为none cachable

## Implementation

`__set_fixmap`

完成IDX(VA)->PA的页表创建，上面interface层的函数，实际就是对这个函数传入不同的pgprot_t的封装

`fixmap_pud`, `fixmap_pmd`, `fixmap_pte`

返回IDX(VA)的各级页表指针，用于创建页表映射时操作这些entry。

`early_fixmap_init`

在early boot阶段，初始化fixmap自己页表结构，将bm_pmd, bm_pud, bm_pte挂入一级页表，这时内核一级页表是early boot阶段的init_pg_dir。

在early boot结束后，`paging_init` 会在swapper_pg_dir建立新的页表，fixmap的页表会在这个过程中以memblock的形式被迁移过去。

# Module Test

[test-fixmap](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-fixmap)