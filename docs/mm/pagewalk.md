# pagewalk (ptdump)

pagewalk会遍历整个VA空间，遍历每级页表entry。

pagewalk允许传入 `mm_walk` 结构体，在各级entry遍历过程中插入回调函数。

ptdump利用pagewalk，实现了ptdump的framwork，允许调用者注册一个entry回调函数用来解析和显示entry。

arm64 ptdump作为ptdump framework的调用者，在调用ptdump时实现了自己的entry解析和显示回调，并在debugfs提供了一个文件节点。

## Files

```
- mm/pagewalk.c				# Page walk implementation
- mm/ptdump.c				# Page dump framework (do page walk for dump)
- arch/arm64/mm/ptdump.c		# Page dump framework user (implement entry parse and print for arm64)
- arch/arm64/mm/ptdump_debugfs.c	# Create debugfs node
```

## Functions

### pagewalk

`struct mm_walk_ops`

pagewalk时，注册的各级entry回调函数

`struct mm_walk`

pagewalk时，提供给各级entry回调函数使用的上下文数据

`walk_page_range`

遍历mm strcut定义的VMA空间的table entries。

`walk_page_range_novma`

遍历全部page table entries

`walk_page_vma`、`walk_page_mapping`

遍历vm_area_struct、address_space对应的VMA区域的table entries

### ptdump（framework）

`struct ptdump_state`

ptdump操作使用的对象，保存了要dump的 `range` 和entry回调函数 `note_page`

`ptdump_walk_pgd`

ptdump整个页表，会调用pagewalk，定义出pagewalk需要的mm_walk，并添加ptdump自己的entry callback，在callback中，会去调用 `note_page` 来完成ptdump的打印。

### ptdump（arm64 implement）

ptdump arm64封装了ptdump，是ptdump的调用者，实现了arm64平台的ptdump解析和打印功能。

`struct ptdump_info`

arm64 ptdump操作使用的对象，主要包括了 `mm_struct` 待dump的页表空间

`ptdump_walk`

dump整个页表，在调用ptdump framework时，arm64 ptdump由自己的 `note_page` 回调函数，会将打印写入参数传入的 `seq_file` 。

`ptdump_init`

创建出debugfs文件节点。

### debugfs

创建出debugfs文件节点，并添加show函数，在open时调用arm64 ptdump的 `ptdump_walk` 将数据dump到自己文件节点中。

## Usage Test

### Dump kernel page table

1. Enable page table debugging
```
CONFIG_PTDUMP_DEBUGFS=y
```

2. Mount debugfs
```
mount -t debugfs nodev /sys/kernel/debug
```

3. Dump kernel page table
```
cat /sys/kernel/debug/kernel_page_tables
```

### Dump process page table

```
cat /proc/<id>/mem
```

## Module Test

[test-pagewalk](https://github.com/kernel-cyrus/test-modules/tree/master/test-pagewalk)