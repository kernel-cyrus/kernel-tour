# tlb

页表转换，会经过 VA - TLB - Page Table - PA 的过程，TLB中保存了VA-PA映射的缓存，如果访问TLB中已缓存的VA，则会跳过Page Table的过程。

当Page Table更新后，TLB中就可能存在“过时”的映射，这就需要对TLB进行刷新，Invalidate这些缓存的映射，这个操作是由软件手动调用Kernel接口完成的。（因为更新页表本身就是软件的行为，软件自然知道什么时候该去invalidate tlb中的数据）

## Files

```
- /arch/arm64/include/asm/tlbflush.h	# ARM tlb flash interface
```

## Functions

内核中提供了通用的TLB Invalidate接口，每个硬件实现下还由各自更加复杂的刷新接口。

`flush_tlb_all`

将所有tlb数据invalidate，执行后tlb会被清空，所有page table修改都会生效。

`flush_tlb_mm(mm_struct)`

将某进程空间相关的tlb数据invalidate(mm_struct)，执行后，该地址空间的转换都会走页表。

mm_struct表示了一个完整页表空间，一般代表内核空间或这一个进程空间。

`flush_tlb_range(vma, start, end)`

将vma区域start-end这段范围的VA从tlb中invalidate

vma代表了一块已经做了映射的VA区域。

`flush_tlb_page`

将vma区域的一个page从tlb invalidate
