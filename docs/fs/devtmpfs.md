# devtmpfs

devtmpfs就是/dev，是Linux设备的控制文件系统，设备在注册时，会在devtmpfs中创建文件节点。

devtmpfs基于tmpfs或者ramfs（在没有开启tmpfs的情况下），也是一个纯ram做backend的文件系统。

其提供的功能主要包括三部分

- 初始化： 会在Kernel Module ini开始前，将devtmpfs注册到系统里
- 添加删除节点： device_add函数，会调用devtmpfs的create/remove node接口，来添加文件节点
- devtmpfsd： 一个kthread，用来handle添加删除节点的request

关系上，可以认为devtmpfs是内核设备框架实现的一个子系统，用于可视化展示device node并对用户空间提供device的操作接口。

devtmpfs相当于一个在device创建时自动mknod的过程，如果没有devtmpfs，可以手动mknode

## Files

```
- /drivers/base/devtmpfs.c	# devtmpfs implementation
```

## Functions

`devtmpfs_init`, `devtmpfs_setup`, `devtmpfs_mount`

在driver初始化前，注册devtmpfs，完成devtmpfs的初始化。

`devtmpfsd`

devtmpfs 内核线程，用于handle create/remove request

`devtmpfs_create_node`, `devtmpfs_delete_node`

对外暴露的节点添加删除接口，主要由 `device_add` `device_del` 使用。

## Kconfig

`DEVTMPFS`

控制devtmpfs功能编译

`DEVTMPFS_MOUNT`

启动时自动mount devtmpfs到/dev