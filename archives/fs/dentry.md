# dentry

一个路径字符串，在内核中会被解析为一组路径节点object。dentry就是路径中的一个节点。

dentry被用来索引和访问inode，每个dentry对应一个inode，通过dentry可以找到并操作其所对应的inode。

与inode和super block不同，dentry在磁盘中并没有实体储存，dentry链表都是在运行时通过解析path字符串构造出来的。

**dentry cache**

由于每次通过path构造出dentry是一个费时过程，且相同路径没必要每次重复解析。开发者为dentry添加了一个缓存，dentry cache(或dcache)

dcache是一个slab cache，在 `dcache_init` 过程中被初始化，保存在 `dentry_cache` 全局变量中，所有dentry会通过 `d_alloc` 接口，从这个缓存池分配。

`dentry_hashtable` 是dcache的索引表，`d_lookup` 函数利用这个表可以快速找到path string对应的被缓存的dentry object。

**dentry status**

dentry有三种状态：
- used： 指向一个有效inode，且d_count不为0（有效的）
- unused： 指向一个有效inode，但d_count为0（缓存中）
- negtive： 指向一个NULL inode（无效的，缓存中）

inode中的i_dentry，保存了那些正在使用中的dentry。

dentry所在super block的s_dentry_lru中，保存了可以被随时回收释放的dentry。

## Files

```
- /include/linux/fs.h		// dentry declares
- /fs/dcache.c			// dentry functions
- /fs/namei.c			// path framworks, caller of dentry / dcache
```

## Variables

`dentry_cache`

dentry slab cache

`dentry_hashtable`

dentry lookup index hashtable

## Structure

```
dentry
d_name		// 本级路径节点名
d_parent	// 指向上级目录节点dentry
d_inode		// 本级目录节点对应的文件inode
d_sb		// 关联的super block
d_op		// dentry操作
d_mounted	// 是否是一个mount point
d_count		// 使用引用数量，refcount
```

```
dentry_operations
// Called by vfs framework, when execute certain produre, implemented by filesystem, it's the filesystem duty to decide how to handle dentry operations.
```

## Functions

`dcache_init`

初始化dentry_cache，dentry_hastable

`d_lookup`

从dentry_hastable中，通过路径找到对应被缓存的dentry

`d_alloc`

从dentry_cache中申请一个dentry object

`d_instantiate`

将inode attach到一个dentry上，由于所有inode都是通过dentry访问操作的，这个过程类似于操作完inode后通过dentry返回结果

`d_set_d_op`

为dentry设置dentry operation

## Usage Test

查看全局dentry cache统计信息

```
cat /proc/sys/fs/dentry-state	// dentry, unused, ..., negative, dummy
```

这些信息在 `dentry_stat` 中定义。