# filesystem

vfs filesystem part，主要定义了一个全局链表，包含了系统全部注册的文件系统，并在proc下提供了节点。

mount时，会调用对应文件系统的mount和umount回调函数，实现者需要根据传入的device初始化出super block，或在umount时销毁。

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
	mount				// do_mount callback
		需要根据传入的device name，创建super block，找到root inode并返回root dentry
		*fs_type		// this filesystem | passed in
		*dev_name		// mount device name | passed in
		*data			// mount options | passed in
	kill_sb				// do_umount callback
		需要清理掉super block占用的内存
		*super_block		// super block of this mount
```

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

## Module Test

[test-filesystem](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-filesystem)