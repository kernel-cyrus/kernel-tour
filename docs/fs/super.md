# superblock

> "A superblock object represents a mounted filesystem."

Super Block是一个文件系统的具体实现，在挂载filesystem时，会创建出这个filesystem对应super block的instance。每个filesystem有其对应的super block实现。

基于block device的filesystem，super block保存在磁盘头的固定sector中。

基于memory的pseudo filesystem，super block在挂载时创建并保存在内存中。

## OO Design

整个VFS都是基于OO思想设计，以C语言实现的。框架定义基类及接口，FS继承并实现子类。

由于C语言没有真正的Class，在实现时采用operations pointer + struct object的方式，来当作类method。

VFS中定义的各种基本struct和struct中定义的operation，就是这些Class的C语言实现。

## Files

```
- /fs/super.c	// Super base class and framework
- /fs/libfs.c	// Generic filesystem operations implementation （各文件系统可能复用的operation通用实现）
```

## Structure

`super_block` - `super_operations`

```
alloc_inode	// Alloc inode
free_inode	// Free inode
dirty_inode	// Called when inode is modified
write_inode	// Write inode to disk
delete_inode	// Delete inode from disk
put_super	// Called when unmount
write_super	// Write super block to disk
statfs		// Get filesystem statistics
remount_fs	// Called when remount
show_options	// Display mount options in /proc/mounts
```
