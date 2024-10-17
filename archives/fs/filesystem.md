# filesystem

vfs filesystem part，主要定义了一个全局链表，包含了系统全部注册的文件系统，并在proc下提供了节点。

mount时，会调用对应文件系统的mount和umount回调函数，实现者需要根据传入的device初始化出super block，或在umount时销毁。

fs_context 引入了一种新的super block创建方式（待整理），这个patch在2019年被合入，老的接口仍然可以兼容使用，但是目前大部分文件系统实现已经切换到了fs_context的新方式，见： [VFS: Introduce filesystem context](https://lwn.net/Articles/780267/)，[Filesystem Mount API](https://www.kernel.org/doc/html/latest/filesystems/mount_api.html)

## Files

```
- /include/linux/fs.h
- /fs/filesystems.c
```

## Structure

```
file_system_type
	name				// filesystem name
	fs_flags			// feature flags recognized by vfs framework
	fs_context			// ? new mount api
	parameters			// ? new mount api
	mount				// do_mount callback, create super block
		需要根据传入的device name，创建super block，找到root inode并返回root dentry
		*fs_type		// this filesystem | passed in
		*dev_name		// mount device name | passed in
		*data			// mount options | passed in
	kill_sb				// do_umount callback, destory super block
		销毁super block
		*super_block		// super block of this mount
```

## Functions

mount时，一般不手写mount过程，而是根据mount的磁盘类型，使用通用mount接口，只需传入fill_super_block的过程即可。

`mount_bdev`

mount a filesystem residing on a block device

`mount_nodev`

mount a filesystem that is not backed by a device

`mount_single`

mount a filesystem which shares the instance between all mounts

## Variables

`file_systems`

全局filesystem链表，包含系统全部的已注册的filesystem。

## Functions

`register_filesystem`

将filesystem注册到filesystem全局链表

`unregister_filesystem`

从filesystem全局链表移除

`proc_filesystems_init` `filesystems_proc_show`

/proc/filesystems节点，遍历显示全局filesystem链表

## Usage Test

List all filesystem in kernel

```
cat /proc/filesystem
```

Mount a filesystem

```
mount -t <filesystem> <device> <mountpoint>
```

Mount a file backend block device

```
dd if=/dev/zero of=loopbackfile.img bs=1M count=10
losetup -fP loopbackfile.img
mkdir /mnt/test
mount -t testfs /dev/loop0 /mnt/test
```

Show mount options

```
cat /proc/mounts
```

## Module Test

[test-filesystem](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-filesystem)