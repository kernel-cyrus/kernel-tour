# mmu

本文关于页表建立过程及相关函数。

## Files

```
- /arch/arm64/mm/mmu.c			# 页表操作高级接口
- /arch/arm64/kvm/hyp/pgtable.c		# 页表底层定义及操作接口
```

## Functions

`paging_init`

页表创建函数