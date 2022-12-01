# Direct

Direct DMA Mapping，在没有iommu（smmu）存在的情况下，memory的pa到ba是直接一整块区域映射过去的，大部分时候pa=ba，有些架构实现上，总线地址可能会做平移转换，ba=pa+offset。这个映射会保存在device->dma_range_map中，后面我们将pa到ba的这种平移转换关系叫做direct映射。

direct通路就是for没有iommu情况的dma buf内存申请。

direct主要完成了两个功能，一个是提供出这种direct映射的pa、ba转换接口，第二是提供出direct情况的连续内存申请和映射功能。

## Files

```
/include/linux/dma-direct.h	# direct dma mapping pa<=>ba换算接口
/kernel/dma/direct.c		# direct dma buf alloc
```

## Functions

`phys_to_dma`、`dma_to_phys`

pa <=> ba by direct mapping

`dma_direct_alloc`、`dma_direct_free`

direct情况的通用DMA内存申请接口

根据是否是coherent device，到coherent pool或者normal memory中申请内存，并根据direct mapping返回pa、ba

函数详细流程见dma-mapping

```
dma_direct_alloc
	
	# For non-coherent device
	dma_alloc_from_global_coherent	（Global Coherent Pool）
		alloc from global dma pool
	dma_direct_alloc_from_pool	（Global Atomic Pool）
		alloc from 3 dynamic pool

	# For coherent device
	__dma_direct_alloc_pages	（Global CMA Pool）
		alloc from cma
```

`dma_direct_alloc_pages`、`dma_direct_free_pages`

direct情况的通用DMA物理页申请接口