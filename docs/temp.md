# zram


**zcom**

zram的compressor在每个cpu有独立stream object，每个核可以独立并行处理压缩过程。

zram的压缩算法定义在zcomp的显式list中，这导致增加算法必须要修改这个静态列表。

## Usage Test


查看compressor stream数量（实际返回的是CPU数）
```
cat /sys/block/zram0/max_comp_streams
```

查看、设置压缩算法

```
cat /sys/block/zram0/comp_algorithm
echo lzo > /sys/block/zram0/comp_algorithm
```

