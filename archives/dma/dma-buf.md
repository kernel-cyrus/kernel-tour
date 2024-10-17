# dma-buf

Dma buffer是一块允许在CPU和其他子系统或IO设备间sharing的buffer。

Dma-buf是这个buffer的描述和使用框架。框架提供了对dma-buffer的描述结构体，和基于file的dma-buf使用接口，同时完成了操作时刷cache的动作。

dma-buf框架实现了一个dma-buf的文件系统(filesystem)，这个文件系统直接通过调用挂载接口挂载到了内核（没有提供挂载节点），每个dma-buf实例，会在文件系统中创建出inode及file，并将fd返回给调用者。调用者通过对file的ioctrl、map等操作，来操作dma-buf。

dma-buf框架有两个概念，exporter（dma-buf的生产者，比如一个dma-heap），importer（dma-buf的使用者，比如上层应用）

dma-buf实际是一个抽象类，只定义了接口，和使用流程，内部的数据结构，都是抽象的。也就是说dma-buf ops，可以理解为针对与"dma-buf"的操作，但是这个"dma-buf"是个什么数据结构，由exporter来实现。exporter实现后，只要提供出给importer使用的各种importer关心的数据类型的转化接口就可以，至于内部数据结构，只保存在private_data里，由exporter自己适配处理，外部没人关心。

理解dma-buf的抽象，需要先了解清楚这种抽象提供给importer后，设计目标是让其如何使用。

dma-buf只是提供了dma buffer的管理使用规范化的通用访问接口框架，使用者可以通过框架来使用dma buffer（读取、写入、修改buffer数据），只要按照框架规范的操作流程来操作，框架会保证cache一致性。这个框架并不设计DMA搬运的事，只负责buffer的使用和操作。buffer修改好后，如果需要做DMA搬运，使用DMA engine接口搬运即可。（dma-mapping is for sharing，dma-engine is for copying）

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

## Use dma-buf from kernel space

1、准备访问

处理器在内核空间打算访问dma_buf对象前，需要通知生产者。

`dma_buf_begin_cpu_access`

生产者可以确保处理器可以访问这些内存缓冲区，生产者也需要确定处理器在指定区域及指定方向的访问是一致性的。

2、使用DMA Buffer

`dma_buf_kmap`、`dma_buf_kunmap`
`dma_buf_kmap_atomic`、`dma_buf_kunmap_atomic`

3、结束访问

`dma_buf_end_cpu_access`

当importer完成对begin_cpu_access指定范围内的缓冲区访问，需要通知生产者（刷新cache，同步数据集释放资源）。

## Use dma-buf from user space

1、exporter驱动申请或者引用导入的待共享访问的内存。（申请dma内存）

2、exporter驱动调用dma_buf_export()创建dma_buf对象，同时将自定义的struct dma_buf_ops方法集和步骤1中的内存挂载到dma_buf对象中。（生成dma-buf）

3、exporter驱动调用dma_buf_fd()将步骤2中创建的dma_buf对象关联到全局可见的文件描述符fd，同时通过ioctl方法将fd传递给应用层。（通过ioctrl返回dma-buf的fd）

4、应用层将fd传递给importer驱动程序。（应用得到fd）

5、importer驱动通过调用dma_buf_get(fd)获取dma_buf对象。（应用获取buf对象）

6、importer驱动调用dma_buf_attach()和dma_buf_map_attachment()获取共享缓存的信息。（应用通过attach得到最终dma buffer指针，sg_table: table of dma buffer）

7、importer操作这块dma buffer。

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

<https://blog.csdn.net/zhong29/article/details/89644321>