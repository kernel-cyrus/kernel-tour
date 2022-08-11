# procfs

procfs是process fs的缩写，procfs的设计初衷，就是用来dump内核中的进程相关信息的。

目前procfs被用于dump进程和内核结构的信息，内核的各种机制，在procfs中实现了自己的数据结构的dump接口。

已实现的这些文件节点的详细描述，可以查看：[procfs](https://kernel.org/doc/html/latest/filesystems/proc.html)

procfs在vfs基础上，封装了一层framework，使用 `proc_dir_entry` 作为操作对象，对外提供出procfs的接口。

## Files

```
- /include/linux/proc_fs.h	# procfs interface
- /fs/proc/root.c		# procfs filesystem, root init
- /fs/proc/inode.c		# procfs super, inode, file operation
- /fs/proc/generic.c		# procfs framework implementation
- /fs/proc/...			# procfs various nodes implementation
```

## Structures

`proc_dir_entry`

procfs核心操作对象，代码中缩写为pde，procfs提供了接口对 `proc_dir_entry` 对象与其他vfs对象间做转换。（见inode.c中使用pde的相关接口）

## Variables

`proc_inode_cache`

procfs在init时创建的inode slab cache

`proc_dir_entry_cache`

procfs在init时创建的inode pde cache

## Functions

`proc_root_init`

procfs初始化函数，负责注册procfs filesystem，初始化出root节点和其他基本功能节点。

`proc_create`

创建文件节点

`proc_create_single`

创建只读文件节点（只有show函数）

`proc_create_seq`



`proc_mkdir`

`proc_symlink`

## Usage Test


## Module Test

[test-procfs](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-procfs)