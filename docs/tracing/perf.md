# perf

## Build Perf

Build from kernel:

```
ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- make -C tools/perf
```

Install perf to initrd staging folder:

```
ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- make -C tools/perf prefix=$(realpath ~/initrd/) install
```

Pack initrd staging folder to initramfs.cpio.gz:

```
cd ~/initrd/
find . -print0 | cpio --null --create --verbose --format=newc -R root:root | gzip > ../initramfs.cpio.gz
```

## Use Perf

**Event statistics**

```
perf stat -e sched:sched_switch -a sleep 5
```

**Trace Event call stack statistics**

```
perf record -e sched:pelt_cfs_tp -a -g sleep 1
perf report -g
```

**Kprobe call stack statistics**

```
# Create kprobe
cd /sys/kernel/debug/tracing
echo 'r:avgload3 __update_load_avg_cfs_rq $retval' > kprobe_events
echo 1 > events/kprobes/avgload3/enable

# Perf record kprobe
perf record -e kprobes:avgload3 -a -g sleep 1

# Perf report kprobe
perf report -g
```

统计一个函数在系统中的调用场景和所有调用路径，协助阅读代码，非常实用。