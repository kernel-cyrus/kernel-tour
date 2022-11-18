# dma-mapping

**coherent framwork**

coherent device
non-coherent devices



dma_alloc_coherent
	dma_alloc_attrs
		# 从device dma memory分配
		dma_alloc_from_dev_coherent

		#
		dma_direct_alloc
			
			
			dma_direct_alloc_no_mapping
				__dma_direct_alloc_pages
					alloc from cma

			# for non-coherent device
			dma_alloc_from_global_coherent
				alloc from global dma pool
			dma_direct_alloc_from_pool
				alloc from 3个 pool

			# for coherent device
			__dma_direct_alloc_pages
				alloc from cma

		# 使用iommu注册进来的dma_map_ops来alloc
		ops->alloc
