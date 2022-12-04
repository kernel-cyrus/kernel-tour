# dma-buf

dma heap, 从dma heap alloc dma buf

将一个dma pool或者一大块dma buffer注册成一个dma heap，并实现这个heap的alloc接口，注册后heap会暴露一个文件节点，供用户空间申请dma buf。heap框架实现了标准ioctrl流程，和dma buf的创建流程，用户通过ioctrl可以直接从fd中申请到dma buf，并获得dma buf的fd。

dma buf是一块允许sharing的buffer，dma buf可以实现这个dma buf的操作回调接口，用来控制dma buf的owner转移，并实现刷cache操作。

## Reference

</https://docs.kernel.org/driver-api/dma-buf.html#shared-dma-buffers>

<https://blog.csdn.net/hexiaolong2009/article/details/102596744>

<https://lwn.net/Articles/474819/>