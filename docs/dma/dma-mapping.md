# dma-mapping

DMA Buffer是一块供DMA Slave Device使用的Buffer。这块通过DMA Engine搬运使用的DDR Buffer。这个Buffer在CPU侧映射出了VA，可以写入或读取里面数据，同时通过

dma mapping用来从dma pool中申请或者map出dma buffer。



**coherent framwork**

for coherent device（direct）
for non-coherent devices（coherent）

## Functions

### Consistent API

```
dma_alloc_coherent（dma_alloc_attrs）

	# 如果Device绑定了Reserved Memory		（Device Coherent Pool）
	dma_alloc_from_dev_coherent
	* Device Memory一定不经过IOMMU，不会被IOMMU映射
	* 或者说使用IOMMU映射一定不能绑定Device Memory，因为Page是动态申请的

	# 如果Device没有绑定Reserved Memory，则从Global Pool/Memory申请

		# 如果Device没有绑定IOMMU，使用Direct申请接口
		dma_direct_alloc
			
			# For non-coherent device
			dma_alloc_from_global_coherent	（Global Coherent Pool）
				alloc from global dma pool
			dma_direct_alloc_from_pool	（Global Atomic Pool）
				alloc from 3 dynamic pool

			# For coherent device
			__dma_direct_alloc_pages	（Global CMA Pool）
				alloc from cma

		# 如果Device绑定了IOMMU，使用IOMMU申请接口	（Alloc Pages and Map IOMMU）
		dma_ops->alloc（iommu_dma_alloc）
```

### Streaming API

**dma mapping APIs**

dma_map_single

dma_map_page

dma_map_sg

## Reference

DMA Mapping

<https://www.kernel.org/doc/Documentation/DMA-API-HOWTO.txt>

DMA Mapping API

<https://www.kernel.org/doc/Documentation/DMA-API.txt>