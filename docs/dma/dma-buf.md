# dma-buf

dma heap, 从dma heap alloc dma buf

将一个dma pool或者一大块dma buffer注册成一个dma heap，并实现这个heap的alloc接口，注册后heap会暴露一个文件节点，供用户空间申请dma buf。heap框架实现了标准ioctrl流程，和dma buf的创建流程，用户通过ioctrl可以直接从fd中申请到dma buf，并获得dma buf的fd。

dma buf是一块允许sharing的buffer，除了提供dma buf的操作回调接口，用来控制dma buf的owner转移，并实现刷cache操作，还提供了fd供用户空间操作和共享。

dma-buf实际是一个抽象类，只定义了接口，和使用流程，内部的数据结构，都是抽象的。也就是说dma-buf ops，可以理解为针对与"dma-buf"的操作，但是这个"dma-buf"是个什么数据结构，由exporter来实现。exporter实现后，只要提供出给importer使用的各种importer关心的数据类型的转化接口就可以，至于内部数据结构，只保存在private_data里，由exporter自己适配处理，外部没人关心。

理解dma-buf的抽象，需要先了解清楚这种抽象提供给importer后，设计目标是让其如何使用。

## Reference

</https://docs.kernel.org/driver-api/dma-buf.html#shared-dma-buffers>

<https://blog.csdn.net/hexiaolong2009/article/details/102596744>

<https://lwn.net/Articles/474819/>

<https://blog.csdn.net/Linux_Everything/article/details/96472705>