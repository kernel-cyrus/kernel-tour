# Pool

coherent_pool动态创建机制（Global Atomic Pool）。

允许通过启动参数：coherent_pool=<size>，在启动阶段动态创建出三个Global DMA Pool（dma, dma32, kernel）

每个pool由一个全局变量指针（atomic_pool_xxx），和一个全局变量保存size。

整个机制提供了dma_alloc_from_pool，可以通过传入gfp来决定从哪个pool分配内存。

Atomic Pool的内部，使用generic mem pool分配器来管理（genalloc）

每个pool的原始内存，通过调用atomic_pool_expand，从cma或者直接通过alloc_pages从buddy申请。

Pool在初始化时，只申请了物理页，还没有做va映射。在device从pool中申请内存时，才通过dma_common_contiguous_remap（调用vmap）为page分map出va。这个map的内存也是non-cache或者说coherent的。

在debugfs中，提供了dma_pools目录来dump三个pool的size