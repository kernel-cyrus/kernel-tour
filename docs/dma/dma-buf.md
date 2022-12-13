# dma-buf

Dma buf是一块允许在CPU和其他子系统或IO设备间sharing的buffer

dma-buf框架实现了一个dma-buf的文件系统(filesystem)，这个文件系统直接通过调用挂载接口挂载到了内核（没有提供挂载节点），每个dma-buf实例，会在文件系统中创建出inode及file，并将fd返回给调用者。调用者通过对file的ioctrl、map等操作，来操作dma-buf。

dma-buf实际是一个抽象类，只定义了接口，和使用流程，内部的数据结构，都是抽象的。也就是说dma-buf ops，可以理解为针对与"dma-buf"的操作，但是这个"dma-buf"是个什么数据结构，由exporter来实现。exporter实现后，只要提供出给importer使用的各种importer关心的数据类型的转化接口就可以，至于内部数据结构，只保存在private_data里，由exporter自己适配处理，外部没人关心。

理解dma-buf的抽象，需要先了解清楚这种抽象提供给importer后，设计目标是让其如何使用。

## Files

```
- /drivers/dma-buf/dma-buf.c
- /drivers/dma-buf/dma-buf-sysfs-stats.c	# dma-buf sysfs stats
```

## Structure

`struct dma_buf`

dma-buf基本结构体，包含了一个dma-buf的名字、owner、file、节点等信息，最重要的是其绑定的ops

`struct dma_buf_ops`

dma-buf虚基类定义的接口，由继承类实现（在dma-heap中，或者独立实现）。

```
attach
detach
pin
unpin
map_dma_buf
unmap_dma_buf
release
begin_cpu_access
end_cpu_access
mmap
vmap
vunmap
```

`dma_buf_export_info`

创建（export）一个dma-buf所使用的参数结构体

## Variables

`dma_buf_list`

全局dma-buf链表

## Functions

`dma_buf_init`

创建出dma-buf filesystem，并挂载到kernel，拿到mount实例。

`dma_buf_export`

创建一个dma-buf。使用dma_buf_export_info作为参数，从dma-buf filesystem创建出indoe，及对应的file，保存到dma-buf中返回。创建的dma-buf会加入全局链表。

`dma_buf_fd`

返回dma-buf对应的fd（给用户态或内核态文件操作使用）

`dma_buf_get`

返回fd对应的dma-buf（通过fd查找到dma-buf实例）

`file_operations`

```
.release	= dma_buf_file_release,
.mmap		= dma_buf_mmap_internal,
.llseek		= dma_buf_llseek,
.poll		= dma_buf_poll,
.unlocked_ioctl	= dma_buf_ioctl,
.compat_ioctl	= compat_ptr_ioctl,
.show_fdinfo	= dma_buf_show_fdinfo,
```

通过fd操作dma-buf的回调函数

`dma-buf methods`

```
dma_buf_dynamic_attach
dma_buf_detach
dma_buf_pin
dma_buf_unpin
dma_buf_map_attachment
dma_buf_unmap_attachment
dma_buf_move_notify
dma_buf_begin_cpu_access
dma_buf_end_cpu_access
dma_buf_mmap
dma_buf_vmap
```

通过dma-buf直接调用其method

## Use DMA-BUF from Kernel

...（待完成）

## Use DMA-BUF from Userspace

...（待完成）

## Debugfs

`/sys/kernel/debug/dma-buf/bufinfo`

dump所有dma-buf信息，包括size, flags, mode, count, inode, expoter

## Sysfs

`/sys/kernel/dmabuf/buffers/<dma-buf>`

enable CONFIG_DMABUF_SYSFS_STATS

每个dma-buf都会在这里创建统计节点，包含所有dma-buf的exporter、size

## Reference

</https://docs.kernel.org/driver-api/dma-buf.html#shared-dma-buffers>

<https://blog.csdn.net/hexiaolong2009/article/details/102596744>

<https://lwn.net/Articles/474819/>

<https://blog.csdn.net/Linux_Everything/article/details/96472705>

<https://blog.csdn.net/u013554213/article/details/84100852>