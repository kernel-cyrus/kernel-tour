# sysfs

> sysfs is a filesystem for exporting kernel objects(kobject)

sysfs是系统中所有kobject的layout。每个kobject，对应sysfs中的一个目录。kobject可以创建自己的attribute，每个attribute在sysfs中对应一个文件。

每个kobject在kobject->ktype->sysfs_ops中，定义show、store函数，kobject下的所有attribute文件，都共用这个show、store，来响应cat、echo操作。

一般情况下，各个模块会定义自己私有数据结构，在私有数据结构中定义出自己私有数据结构的show、store，这个二级show、store，通过sysfs_ops中的show、store转发过来。（二级数据结构封装）

各个子系统的实现中，都用这种二级数据结构封装的方式实现了自己的data、attr结构体，相关定义、初始化宏，以及操作函数。比如device、bus、driver，这些kobject都有自己的二级封装。

## Files

```
- /include/linux/sysfs.h	# sysfs interface
- /fs/sysfs/*			# sysfs implementation
```

## Interface

### original interface

`sysfs_create_file`

创建一个attr节点。

`sysfs_create_group`

创建一批attr节点（attr group，一个attr数组）

`__ATTR_XXX`

简易的二级数据结构赋值宏

### device interface

`struct device_attribute`

用__ATTR定义device attribute

`device_create_file`

创建/dev/<device>/<attrs>

## Module Test

kobject and attr group:

[test-kobject](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-kobject)

my_data / my_attr implementation:

[test-sysfs](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-sysfs)