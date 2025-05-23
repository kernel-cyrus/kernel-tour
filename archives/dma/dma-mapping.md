# dma-mapping

DMA Buffer就是一块供IO Device使用的Buffer。

这些Device大致可以分为三类，一个是DMA Engine Device，也就是帮助CPU来完成DMA搬运的物理设备。第二个是IO Device，也就是需要DMA Engine来帮其完成搬运的DMA Slave Device。第三类就是总线上的其他（Subsys的）Master，他们需要跟CPU共享一块Buffer。

一般情况下，这些Device挂在“外部”总线上，可以认为他们跟CPU不在同一总线（比如各自Subsys内部），这些外部设备通过自己的Bus Address（BA）来访问自己Subsys的资源以及DDR。一些架构上，这些Device与DDR之间有SMMU映射，访问DDR的BA实际是经过SMMU映射的IOVA。

任何一类外设想要访问DDR，都需要一块DMA Buffer。比如给一个IO设备映射一块DDR Buffer（通过IOVA）；让DMA设备IO设备（通过BA）搬运DDR数据（通过IOVA）；或者另一个Master也想要访问DDR（通过IOVA），独自使用一块DDR内存或者用来与CPU共享。

这种提供给外设访问的DDR Buffer，不论是用于自己使用，还是用于DMA传输，或者用于独占或共享，都叫做DMA Buffer。

DMA Mapping，就是从dma pool中申请或者为已申请内存map出dma buffer。

DMA Mapping包括两类功能，一个是提供出DMA Buffer的通用申请接口"dma_alloc_coherent"，这个接口会根据device的coherent的类型，device是否绑定了dma pool，global dma pool的情况，选择出合适的DMA Buffer申请通路，返回DMA Buffer。另一个功能是为已有的Page提供出DMA Buffer的Map接口，将已申请Page映射为DMA Buffer。

不论是哪类功能，在申请或映射DMA Buffer时，都会自动调用IOMMU（如果Device有绑定IOMMU），完成IOVA的映射。

**DMA Pools**

DMA提供了几个DMA Buffer Pool。根据Device是否coherent（自己能保证cache一致性），是否有iommu，静态reserve还是启动后分配这几个属性，用户可以选择使用哪个Pool提供的接口，或者由Alloc接口来自动选择从哪个Pool进行分配。

- Device DMA Pool

	DTS中直接为Device绑定的一块Reserved连续物理内存，设备独占，Non-Cacheable（shared-dma-pool, no-map）

- Device CMA Pool
	
	DTS中直接为Device绑定的一块Reserved连续物理内存，不使用时允许在这里申请Moveable Pagle，Non-Cacheable（shared-dma-pool, reusable

- Global DMA Pool

	DTS中在Reserved Memory中静态声明的一个Global Pool，Non-Cacheable（linux,dma-default）

	允许Device通过这个共享Pool动态申请DMA Buffer，允许多个Device共享，提高内存利用效率。

- Global CMA Pool

	DTS中Reserved Memory中静态声明的一个Global Pool（linux,cma-default）

	通过dma_alloc_coherent申请的cma pages被映射为Non-Cacheable，通过dma_alloc_noncoherent申请到的cma page被映射为Cacheable

	允许Device通过这个共享Pool动态申请DMA Buffer，在不使用时允许在这里申请Movable Page，避免内存浪费。

- Global Atomic Pool

	通过启动参数动态初始化的一个Global Pool，page来源于CMA或者Buddy，Non-Cacheable（coherent_pool=size）

- IOMMU Page Mapping

	IOMMU存在的情况下，不再需要物理连续，直接从buddy申请物理页然后通过IOMMU映射出连续IOVA。

**Coherent or Non-Coherent**

Coherent Device在硬件上自己可以保证cache一致性，DTS的device节点通过"dma-coherent"来将一个设备声明为Coherent Device。

Non-coherent Devices硬件无法保证一致性，需要将DMA Buffer映射为Non-Chaceable。

内存申请时，会根据Device是否能够自己保证Cache一致性来选取不同的申请路径和Pool。

## Files

```
- /include/linux/dma-mapping.h		# DMA Allocation & Mapping Interface
- /kernel/dma/mapping.c			# DMA Mapping top implementation

```

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
			dma_alloc_from_global_coherent	（Global DMA Pool）
				alloc from global dma pool
			dma_direct_alloc_from_pool	（Global Atomic Pool）
				alloc from 3 dynamic pool

			# For coherent device
			__dma_direct_alloc_pages	（Global CMA Pool）
				alloc from cma

		# 如果Device绑定了IOMMU，使用IOMMU申请接口	（Alloc Pages and Map IOVA）
		dma_ops->alloc（iommu_dma_alloc）
```

### Streaming API

**dma mapping APIs**

dma_map_single

dma_map_page

dma_map_sg - sg is used to map list of page to iova

dma_sync_single_for_cpu() / dma_sync_single_for_device()

dma_sync_sg_for_cpu() / dma_sync_sg_for_device()

## Reference

DMA Mapping

<https://www.kernel.org/doc/Documentation/DMA-API-HOWTO.txt>

DMA Mapping API

<https://www.kernel.org/doc/Documentation/DMA-API.txt>


API
<https://blog.csdn.net/jasonchen_gbd/article/details/79462064>
