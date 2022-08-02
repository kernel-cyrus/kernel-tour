# inode

> "inode is a file or directory of a filesystem"

inode是文件系统中文件和目录对象。

**inode and filesystem**

每个inode在一个挂载的文件系统中，有一个唯一编号：inode ID，文件的inode ID可以通过 `ls -i` 查看。

一个filesystem，inode ID的总量是固定的，可以通过 `df -i` 查看inode余量。

**inode and blocks**

inode保存在内存中，inode所表示的文件的内容，保存在磁盘中（或内存）。

一个inode关联了其磁盘中物理block的链表，通过inode就可以从磁盘对应的sectors中读取到最终的文件内容。

磁盘的物理block一般为512字节，而访问磁盘时，为提高性能，一般会一次读取8个block，叫做一个4K IO Block

**inode and dentry**

inode在VFS中，通过dentry进行索引和操作。shell的文件访问命令，会通过系统调用，经过VFS框架，最终回调到对应的inode operation中。

inode的operation通过传入的dentry和父目录inode，访问和操作出dentry对应的inode，最后通过 `d_instantiate` 将操作后的inode重新绑定到dentry中，完成一次文件操作。

## Files

```
- /include/linux/fs.h
- /fs/inode.c
- /fs/libfs.c			// inode generic operations
```

## Structures

inode结构体很大，但是并非所有property都被使用，使用哪些property取决于文件系统的实现。

这里主要列出与其他vfs class有关联的inode property。

```
inode
i_op		// inode operation
i_sb		// 关联的super block
i_fop		// file operation
i_ino		// inode ID
i_dentry	// 文件的dentry节点
...
```

inode_operations为inode类的方法，用于实现对一个inode对象的具体操作

```
inode_operations
create		// Called by open(2), create(2)
link		// Called by link(2)
unlink		// Called by unlink(2)
symlink		// Called by symlink(2)
mkdir		// Called by mkdir(2)
rmdir		// Called by rmdir(2)
mknod		// Called by mknod(2)
rename		// Called by rename(2)
setattr		// Called by chmod(2)
getattr		// Called by stat(2)
lookup		// Lookup an inode in parent directory
...
```

## Usage Test

获取文件inode信息

```
stat <file / directory>
  File: ../codebase/linux-kernel/mainline/vmlinux
  Size: 508289536（文件大小） Blocks: 992440（物理Blocks Num） IO Block: 4096（IO Blocks Size） regular file
Device: 10302h/66306d	Inode: 1068410（inode ID）  Links: 1（硬链接）
Access: (0775/-rwxrwxr-x)  Uid: ( 1000/   cyrus)   Gid: ( 1000/   cyrus)  （权限）
Access: 2022-08-01 11:31:18.487415643 +0800	（访问时间）
Modify: 2022-07-21 18:29:37.473799911 +0800	（修改时间）
Change: 2022-07-21 18:29:37.473799911 +0800	（修改时间）
```

获取文件inode ID

```
ls -i
```

获取所挂载的filesystem inode总量使用情况

```
df -i
```

## Debug Test

通过ramfs断点，观察inode operation流程

1、创建ramfs

```
mount -t ramfs -o size=20m ramfs /mnt/ram
```

2、断点

```
b ramfs_mknod
```

3、操作文件

```
touch /mnt/ram/test
```

4、观察 `dentry` `dir` `inode` 结构体内容及操作过程