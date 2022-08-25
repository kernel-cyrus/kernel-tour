# zram

**zram block device**

**zram compressor**

zram的compressor在每个cpu有独立stream object，每个核可以独立并行处理压缩过程。

zram的压缩算法定义在zcomp的显式list中，这导致增加算法必须要修改这个静态列表。

**write back**

`CONFIG_ZRAM_WRITEBACK`

目前zram已经支持增加后备磁盘设备，类似zswap，zram可以将idle或者不能压缩的page写入后备磁盘，以节省内存空间。

**memory tracking**

`CONFIG_ZRAM_MEMORY_TRACKING`

这个feature会增加block_state节点，可以dump出zram block device中，每个block的状态。

## Usage Test

动态添加、删除zram磁盘

```
cat /sys/class/zram-control/hot_add		# 添加一块新盘
echo X > /sys/class/zram-control/hot_remove	# 删除zramX
```

查看compressor stream数量（实际返回的是CPU数）
```
cat /sys/block/zram0/max_comp_streams
```

查看、设置压缩算法

```
cat /sys/block/zram0/comp_algorithm
echo lzo > /sys/block/zram0/comp_algorithm
```

设置zram磁盘大小

```
echo 1G > /sys/block/zram0/disksize
```

使用zram磁盘作为swap分区

```
mkswap /dev/zram0
swapon /dev/zram0
swapoff /dev/zram0
```

使用zram磁盘作为普通磁盘

```
mkfs.ext4 /dev/zram1
mount /dev/zram1 /tmp
umount /dev/zram1
```

查看zram磁盘统计数据

```
cat /sys/block/zram<id>/stat
cat /sys/block/zram<id>/mm_stat
cat /sys/block/zram<id>/io_stat
cat /sys/block/zram<id>/bd_stat
```

重置zram磁盘

```
echo 1 > /sys/block/zram0/reset
```

Write back（CONFIG_ZRAM_WRITEBACK）

```
echo /dev/sda5 > /sys/block/zramX/backing_dev	# 设置后备磁盘
...待完成
```

Dump zram磁盘中每个block的信息（CONFIG_ZRAM_MEMORY_TRACKING）

```
cat /sys/block/zram<id>/block_state
```

zram提供了一个前端操作工具：zramctl

```
zramctl -h
```

## Refernce

zram.rst