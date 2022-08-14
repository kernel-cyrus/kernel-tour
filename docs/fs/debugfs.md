# debugfs

debugfs用于给用户空间提供内核信息及调试接口。

与/proc的区别在于，/proc只用于提供进程相关信息。

与/sysfs的区别在于，/sys有one-value-per-file的约束规则

debugfs完全没有任何约束规则，并且封装了各种内核数据类型的接口，利用这些接口，你可以更加轻松创建出某个类型变量对应的文件节点，再使用这些节点轻松dump出这些变量或者修改他们。

## Interfaces

`debugfs_create_dir`

创建debugfs目录

`debugfs_create_file`

创建debugfs文件，指定自己的file operation

`debugfs_create_x/u/8/16/32/64`

创建某种数据变量对应的文件节点

`debugfs_create_bool/ulong/size_t/atomic_t/array`

创建某种数据变量对应的文件节点

`debugfs_create_reg32`

创建寄存器对应的文件节点

## Usage Test

Mount debugfs

```
mount -t debugfs none /sys/kernel/debug
```

## Module Test

[test-debugfs](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-debugfs)