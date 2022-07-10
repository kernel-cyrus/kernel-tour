# ptdump (pagewalk)

pagewalk会遍历整个VA空间，遍历每级页表Entry。

pagewalk允许传入 `mm_walk` 结构体，在各级Entry遍历过程中插入回调函数。

## Files

- mm/pagewalk.c
- mm/ptdump.c
- arch/arm64/mm/ptdump.c
- arch/arm64/mm/ptdump_debugfs.c

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

[test-pagewalk]()