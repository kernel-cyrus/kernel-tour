# contiguous

**CMA**

Buddy系统最大只能分配2M连续物理页，而如果通过Reserve Memory来为Device绑定DMA Pool，当Device没有使用时就会对内存造成浪费。

CMA机制就是为了解决这些问题诞生的。CMA创建的Pool，由于已经从Buddy中Reserved了出来，所以允许超过2M的大页申请。在Device没有使用这里的内存时，允许其他程序在这里申请Moveble的Page。在Device使用时，将这些已分配的Moveble Page迁移走。从而避免内存浪费。

最早的CMA机制就内嵌在DMA Contiguous中，后面为了通用性，将CMA独立了出来。

DMA Contiguous使用CMA机制，可以建立两类CMA Pool，用于DMA Buffer的申请。一个是Global CMA Pool或者叫Default CMA Pool，一个是Device CMA Pool。

**Global CMA Pool**

就是Default DMA Contiguous Pool，当Device没有自己的CMA时，则从这个Global CMA Pool申请DMA Buffer。

Global CMA Pool(Default CMA Pool)可以通过Reserve Memory的"linux,cma-default"+"reusable"定义，也可以通过bootargs:cma动态定义覆盖。

Global CMA Pool通过dma_contiguous_default_area全局变量来管理。

**Device CMA Pool**

Device绑定的Reserve内存，如果做成"no-map"，在不使用时就会造成浪费，所以可以定义成"reusabe"，这样这块reserve内存就不会绑定在dev->dma_mem上，而是被初始化为一个CMA pool，绑定在dev->cma_area上。

在Device申请CMA内存时，就会默认先从Device CMA Pool申请。

**IOMMU**

很多系统有IOMMU的存在，对于这类Device的DMA Buffer，不需要真的Reserve或者从CMA申请真实的物理连续内存，只需正常从Buddy申请足够的Page，在IOMMU中Map出连续的IOVA即可。

## Files

```
- /mm/cma.c
- /include/linux/cma.h
- /kernel/dma/contiguous.c
```

## Variables

`dma_contiguous_default_area`

Global CMA Pool。

## Functions

`rmem_cma_setup`

初始化Global CMA Pool，从Reserve Memory中读取"linux,cma-default"节点，在CMA中创建出CMA Pool，并保存到"dma_contiguous_default_area"全局变量中。

初始化Device CMA Pool，为引用了这个"shared-dma-pool"+"reusable"内存的Device绑定上dev->cma_area。

`dma_alloc_contiguous`

调用CMA接口，从CMA Pool分配内存。

如果Device有自己的私有CMA Pool，则从私有CMA Pool分配内存。

如果没有，则从Default CMA Pool分配内存。

`dma_free_contiguous`

释放CMA内存。

`dma_contiguous_reserve`、`dma_contiguous_reserve_area`

为了实现bootargs。

## Bootargs

DMA Contiguous区域可以使用 `cma=` 设置，比如：

cma=100M 或 cma=100M@0x40000-0x?0000

Bootargs会覆盖Reserve Memory中定义的默认CMA Default设置，并在启动时打印出被覆盖的message。

## Reference

<https://www.cnblogs.com/LoyenWang/p/12182594.html>