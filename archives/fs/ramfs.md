# ramfs, tmpfs

**buff/cache**

VFS所有的object，都在VFS Cache中（inode cache, dentry cache），对文件的操作，会先操作object，然后再刷入block device。ramfs是一个没有后备block device的fs，也就是说，创建一个ramfs，实际只使用了VFS的object cache。

**ramfs**

> Basically, you're mounting the disk cache as a filesystem. 

Linus将Cache封装了一个小型文件系统，这个文件系统就是ramfs。mount一个ramfs时，实际上就是在mount cache，对ramfs的写入操作，实际会写入cache。

由于cache后没有后备物理储存设备，所以数据在cache中不会被刷入到储存设备中，会一直保存在文件系统的cache中。

ramfs只是对Cache做了简单的文件系统封装，其内部实现非常简单，大部分工作都是Linux Cache机制自身完成的。

**tmpfs**

> This virtual memory filesystem is heavily based on the ramfs. It extends ramfs by the ability to use swap and honor resource limits which makes it a completely usable filesystem.

tmpfs在ramfs的基础上，实现了swap后备储存功能，并增加了对挂载空间大小的限制。

`CONFIG_SHMEM` 可以用来控制tmpfs是否添加swap功能，还是仅仅基于ramfs实现基本“tiny-shmem”功能。

**ramfs vs tmpfs**

二者都是以内存（VFS Object Cache）作为后备储存的虚拟文件系统

ramfs：

- /fs/ramfs，ramfs大小设置无效的，ramfs的大小会动态增长，直到用掉全部物理内存。
- ramfs会不使用swap空间

tmpfs：

- /fs/tmpfs，tmpfs可以设置大小，不会超过最大大小
- tmpfs支持swap空间，当ram不足时，可以使用磁盘swap文件作为后备缓存

## Files

```
- /fs/ramfs/inode.c	// ramfs implementation
- /mm/shmem.c		// swap and tmpfs implementation
```

## Usage Test

**Create ramfs**

```
# 创建挂载点
sudo mkdir /tmp/ramfs
sudo chmod 777 /tmp/ramfs
# 创建ramfs/tmpfs
sudo mount -t ramfs -o size=10G myramfs /tmp/ramfs
# 写入测试
sudo dd if=/dev/zero of=/tmp/ramfs/zero bs=4k count=100000
# 读取测试
sudo dd if=/tmp/ramfs/zero of=/dev/null bs=4k count=100000
# 测试结果
free
# 卸载
sudo umount /tmp/ramfs
```

**Create tmpfs**

```
# 创建挂载点
sudo mkdir /tmp/tmpfs
sudo chmod 777 /tmp/tmpfs
# 创建ramfs/tmpfs
sudo mount -t tmpfs -o size=10G mytmpfs /tmp/tmpfs
# 写入测试
sudo dd if=/dev/zero of=/tmp/tmpfs/zero bs=4k count=100000
# 读取测试
sudo dd if=/tmp/tmpfs/zero of=/dev/null bs=4k count=100000
# 测试结果
free
# 卸载
sudo umount /tmp/tmpfs
```