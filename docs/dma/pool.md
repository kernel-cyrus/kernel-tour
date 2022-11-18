# Pool

postcore阶段，通过dma_atomic_pool_init，根据bootarg：coherent_pool，创建出三个dma pool（dma, dma32, kernel），每个pool有一个全局变量指针（atomic_pool_xxx），和一个全局变量size

每个pool的原始内存（物理页，这里还没有做va映射，只是从物理内存管理中分配了物理内存及对应的page struct），通过atomic_pool_expand，从cma或者直接通过alloc_pages从buddy申请。

申请后，通过dma_common_contiguous_remap（调用vmap）为page分配出va

pool使用generic mem pool分配器来管理（genalloc）

每个pool在debugfs中提供了一个节点，来打印size

-----------

pool创建好后，就可以提供通用的pool分配内存接口：dma_alloc_from_pool，根据gfp，从三个pool中选择一个分配物理页