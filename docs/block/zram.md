# zram

zram是一个ram based block device，可以创建出/dev/zram\<id\>设备。写入这个内存磁盘设备的数据，会被自动压缩。可以使用这个设备作为swap backend或者/var中其他缓存文件的后备储存。

整个zram的包括三部分，一个是ram block device，一个是compressor负责压缩，最后是提供给用户空间的操作节点，如控制device的大小、压缩方法等。

**zram block device**

...

**zram compressor**

zram的compressor在每个cpu有独立stream object，每个核可以独立并行处理压缩过程。

zram的压缩算法定义在zcomp的显式list中，这导致增加算法必须要修改这个静态列表。

**write back**

`CONFIG_ZRAM_WRITEBACK`

目前zram已经支持增加后备磁盘设备，类似zswap，zram可以将idle或者不能压缩的page写入后备磁盘，以节省内存空间。

**memory tracking**

`CONFIG_ZRAM_MEMORY_TRACKING`

这个feature会增加block_state节点，可以dump出zram block device中，每个block的状态。

## Files

```
- /drivers/block/zram/zram_drv.c	# zram block device
- /drivers/block/zram/zcomp.c		# zram compress function
```

## Usage Test

使能zram需要开启以下依赖：

```
CONFIG_ZRAM
depends on BLOCK && SYSFS && MMU
depends on CRYPTO_LZO || CRYPTO_ZSTD || CRYPTO_LZ4 || CRYPTO_LZ4HC || CRYPTO_842
```

通过num_devices来传入要创建的zram device数量

```
modprobe zram num_devices=4
```

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