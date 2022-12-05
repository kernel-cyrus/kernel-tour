# dma-heap

dma-heap机制，用于将内核的DMA Pool封装成dma-heap，创建出dma heap的device节点，向用户空间暴露出通用的dma-buf申请接口。

一个DMA Pool，或者一大块DMA Buffer，都可以封装为一个DMA Heap。

DMA Heap创建后，会自动在用户空间创建这个Heap的节点，并提供标准化的IOCTRL操作。

用户空间可以使用这个节点，通过IOCTRL从DMA Pool中申请出dma-buf。

dma-buf在使用后，通过dma-buf自己的release归还给DMA Pool。

\* 这里的DMA Pool不是mm/dmapool，指的是通常意义上的一块DMA内存，或分配器。

封装DMA Heap，需要实现两件事，一个是将DMA Pool封装为dma heap并注册（实现dma-heap），一个是实现其产生的dma-buf的(类)操作（实现dma-buf）

## Files

```
- /include/linux/dma-heap.h
- /drivers/dma-buf/dma-heap.c
- /drivers/dma-buf/heaps/system_heap.c
- /drivers/dma-buf/heaps/cma_heap.c
```

## Structures

`struct dma_heap`

dma heap结构体，包括了dma heap的名字，ops，heap创建的cdev节点，heap对应的device。

`struct dma_heap_ops`

dma heap的dma buf alloc接口

`dma_heap_export_info`

用于实现一个dma heap，需要实现ops，name，private data。

## Variables

`heap_list`

全局dma heap链表

## Functions

`dma_heap_init`

启动时调用

创建出dma_heap device class，后续dma heap都会注册成dma heap device

创建出/sys/kernel/dma_heap，用于dump所有dma_heap的size

`dma_heap_add`

创建出dma heap，挂入全局链表

创建出这个dma heap的device（dma heap device）

在/sys/class/dma_heap/创建出这个heap的device节点

在/dev/dma_heap/创建出device操作节点，为节点绑定dma_heap_fops file operation

`dma_heap_open`

响应dma heap文件节点open

`dma_heap_ioctl`、`dma_heap_ioctl_allocate`

响应dma heap文件节点ioctl，用于ioctrl: dma-buf的alloc申请

最终会通过 `dma_heap_buffer_alloc` 调用dma heap实现的alloc接口来申请dma buf

`dma_heap_get_name`、`dma_heap_get_dev`、`dma_heap_get_drvdata`

heap get接口

## DMA Heaps

**system heap**

system heap用来给用户控件提供出从Buddy System申请dma-buf的操作节点，实现其生成的dma-buf。

**cma heap**

cma heap用来给用户空间提供出从Default CMA Pool申请dma-buf的操作节点，实现其生成的dma-buf。