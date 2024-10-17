# DMA coherent

从DMA搬运时的Cache一致性的角度看，Device分为两类：

- coherent device：硬件（IP）自己可以保证搬运过程的一致性
- non-coherent device：硬件（IP）保证不了，需要页表建立成non-cache的

一个device属于哪类，可以在dts中对应的device节点配置"dma-coherent"来声明，这个参数会保存到device结构体的dma_coherent变量中。在使用dma_alloc_coherent申请DMA Buffer时，会根据device的类型，来选择是否需要申请non-cache的内存，通过软件来保证一致性。

coherent模块就为这些non-coherent的DMA设备提供了两类pool，一个device pool，一个global pool，两个pool的内存在分配时都会被映射成non-cache的。

**Device DMA Pool**

Device DMA Pool是绑定在device上的独占DMA buffer pool

在给device绑定了DMA mem后，dma_alloc_coherent会优先从这个pool中分配内存。

有两种方式可以为device绑定dma memory：

1、Device Pool的Memory在DTS中reserve，在reserve-memory中预留，并在节点中通过配置compatible:"shared-dma-pool"+"no-map"来声明这块reserve memory是为device预留的dma buffer pool memory，在device初始化时，dts中通过memory-region引入这类reserve-memory，会将这块物理内存绑定到device->dma_mem上。

2、可以通过dma_declare_coherent_memory为一块物理内存建立non-cache映射，并直接绑定到device->dma_mem上。

\* dts中如果将"shared-dma-pool"声明为"reusable"，则会创建出device cma pool，详见contiguous。

**Global DMA Pool**

除了支持为每个device绑定上reserved dma memory，coherent还支持建立默认的global dma coherent pool。

dts中命名为"linux,dma-default"的reserve memory会被识别成默认global dma coherent buffer pool，映射后的地址会保存在dma_coherent_default_memory全局变量中。

在通过dma_declare_coherent申请dma buffer时，如果device没有绑定device dma mem，则会从这个global coherent pool申请。（when non-coherent device）

## Files

```
/kernel/dma/coherent.c		# coherent dma buffer pool implementation
```

## Functions

`RESERVEDMEM_OF_DECLARE(dma, "shared-dma-pool", rmem_dma_setup);`

声明在reserve-memory中，定义为complatible:"shared-dma-pool"的处理函数。

这个compatible有两个处理函数，一个将"no-map"的初始化为device独占，contigious中的将定义了"reusabe"的初始化为device cma。

`rmem_dma_setup`

对设置为complatible:"shared-dma-pool"+"no-map"的reserve memory进行初始化，为这个mem node绑定device回调

`rmem_dma_device_init`

为引用了dma mem类型的reserve memory的device初始化device->dma_mem

`dma_init_reserved_memory` - `dma_init_global_coherent`

初始化global dma mem（如果dts中有定义）

`dma_alloc_from_global_coherent`
`dma_release_from_global_coherent`
`dma_mmap_from_global_coherent`

从global mem pool中分配dma buf

`dma_alloc_from_dev_coherent`
`dma_release_from_dev_coherent`
`dma_mmap_from_dev_coherent`

从device mem pool中分配dma buf
提供了一套机制为device分配固定的dma buffer。(non-cache的buffer，为non-coherent device)

`dma_declare_coherent_memory`

将一块物理地址初始化并绑定到device mem上

`dma_init_coherent_memory`

将一块物理内存初始化为coherent，会用MEMREMAP_WC做remap，这个flag会将内存映射为non-cacheable

## Reference

Reserved Memory

<https://www.kernel.org/doc/Documentation/devicetree/bindings/reserved-memory/reserved-memory.yaml>

<https://www.kernel.org/doc/Documentation/devicetree/bindings/reserved-memory/shared-dma-pool.yaml>