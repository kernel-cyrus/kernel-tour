# dma-pool

> CMA is for large DMA buffers, dma pool is for small DMA buffers (smaller than a page)

当申请大量小于一页的small dma buffer时，我们不需要申请通过dma_alloc_coherent申请大量的4K buffer，可以用dma pool创建出连续的小块DMA buffer。

dma pool有点类似kmem cache，可以将一块大的DMA buffer做成dma pool，再通过dma pool提供的alloc接口，从pool中申请出这个pool所设置的大小的DMA buffer。

整个dma pool结构如下图

```
|               			 dma pool                                        |
------------------------------------------------------------------------------------------
  dma_page                       dma_page                       dma_page
 /(vaddr, dma)                  /(vaddr, dma)                  /(vaddr, dma)
[   buf   |   buf   |   buf   ][   buf   |   buf   |   buf   ][   buf   |   -   |   -    ]
 <- size ->
 <-----    allocation   ----->
```

在创建dma pool时，并不会分配任何物理页。alloc from pool时，会根据pool中的dma_page对是否由空闲block，来决定从原有页中分配还是调用dma_alloc_coherent申请新的dma_page。

## Files

```
- /include/linux/dmapool.h
- /mm/dmapool.c
```

## Structures

`struct dma_pool`

一个dma pool，对应一个大块DMA Buffer，可以分配出size为"大小的small dma buffer。当pool的容量不够时，一次申请allocation大小的page，一般为一个page。

`struct dma_page`

一个allocation page。一个pool由多个allocation page组成，allocation page从dma_alloc_coherent中申请，一个allocation page被切割成了多个small block，用来分配small dma buffer。

page_list是整个pool的dma_page链表，vaddr是这个dma_page的va，dma是这个page的iova，in_use代表有无small block分配出去，offset代表最近的空闲的small block offset。

## Functions

`dma_pool_create`

为一个Device申请一块总大小为size，对其为align，分配大小为alloc的DMA Pool，alloc需要小于4096（a page）。

创建的DMA Pool，会绑定到dev->dma_pools。

`dma_pool_alloc`

从dma pool申请大小为alloc的small dma buffer

`dma_pool_zalloc`

申请并清零初始化

`dma_pool_free`

释放small dma buffer

`dma_pool_destroy`

释放dma pool

## Sysfs

每个dma pool，都会在sysfs对应device节点下创建出"pools"节点，以PC上的PCI节点为例：

```
/sys/devices/pci0000:00/0000:00:1a.0$ cat pools
poolinfo - 0.1
ehci_sitd           0    0   96  0
ehci_itd            0    0  192  0
ehci_qh             3   42   96  1
ehci_qtd            3   42   96  1
buffer-2048         0    0 2048  0
buffer-512          0    0  512  0
buffer-128          0    0  128  0
buffer-32           0    0   32  0
```

信息依次为：pool name -- blocks allocated -- blocks per page -- block size (bytes) -- pages allocated

## Reference

Part Ib - Using small DMA-coherent buffers

</Documentation/core-api/dma-api.rst>

<https://blog.csdn.net/flyingnosky/article/details/98747826>