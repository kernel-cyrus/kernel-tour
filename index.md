Kernel-Tour文档重新整理中...

旧版文章见：(https://github.com/kernel-cyrus/kernel-tour)[https://github.com/kernel-cyrus/kernel-tour]

# cpu

1.  CPU Overview
2.  CPU Fetch
3.  CPU Decode
4.  CPU Cache
5.  CPU TLB
6.  CPU Renaming
7.  CPU Dispatch
8.  CPU Execution
9.  CPU Tools <label>(perf, topdown, micro benchmarks, perf-case)</label>

# arm

#### architecture
1.  ARM Instructions
2.  ARM Registers
3.  ARM Atomic
4.  ARM Exceptions
5.  ARM GIC
6.  ARM Arch Timer
7.  ARM MMU
8.  ARM Cache
9.  ARM Barriers
10. ARM Trustzone
11. ARM Virtualization

#### components
1.  ARM DSU
2.  ARM CMN
3.  ARM SMMU
4.  ARM Coresight

#### features
1.  ARM PMU
2.  ARM AMU
3.  ARM SPE
4.  ARM VHE
5.  ARM PAC
6.  ARM PAN
7.  ARM BTI
8.  ARM MTE
9.  ARM MPAM
10. ARM PBHA
11. ARM TRNG
12. ARM RAS
13. ARM SIMD/NEON/SVE
14. ARM Lockstep

# schedule

1.  Overview
2.  Process Create
3.  Process Status
4.  CFS/EEVDF Scheudler
5.  Realtime Scheudler
6.  Deadline Scheudler
7.  Stop Scheudler
8.  Idle Scheudler
9.  Preemption
10. Load Balance
11. Load Tracking <label>(PELT)</label>
12. CPU Frequency Scaling <label>(schedutil)</label>
13. Energy Aware Scheduling <label>(EM/EAS)</label>
14. CPU Isolation
15. Preempt RT

# memory

1.  Overview
2.  Pagetable <label>(mmu, fixmap)</label>
3.  Memblock
4.  Buddy System <label>(pfn, memory model, numa, node, zone, page, folio, buddy, alloc page)</label>
5.  Slab Allocator
6.  Kenrel Mem Allocion <label>(kmalloc, vmalloc, vmap_area)</label>
7.  Userspace Mem Allocion <label>(malloc, mmap, vma, mapletree, pagefault, lru, cow)</label>
8.  Huge Page <label>(hugetlb, thp)</label>
9.  Memory Reclaim <label>(rmap, watermark, direct, kswapd, pte flags, page count, lru, mglru, page cache, swap, oom, lowmem)</label>
10. Memory Compact
11. Memory Debug <label>(kasan, kmemleak, kfence)</label>
12. Meminfo
13. DMA Buf <label>(dma_alloc_coherent, dma buf, dma heap)</label>

# filesystem

1.  Overview <label>(filesystem, super, inode, dentry)</label>
2.  Persudo FS <label>(proc, sys, debug, conig)</label>
3.  RAMFS, TMPFS
4.  EXT4
5.  F2FS
6.  EROFS
7.  FUSEBPF
8.  aio
9.  io_uring
10. select / pool / epoll

# block

1.  Overview
2.  ZRAM
3.  RAMDISK

# lock

1.  spinlock
2.  mutex
3.  semaphore
4.  rwsem
5.  rcu
6.  futex
7.  rt mutex
8.  lock debug <label>(lockdep, lockstat)</label>

# time

1.  Arch Timer
2.  Clock Source
3.  Timekeeping
4.  Clock Event Device
5.  Tick Device <label>(tick device, sched tick, tick broadcast, tickless, jiffies)</label>
6.  Legacy Timer
7.  Hi-Res Timer
8.  Time Functions <label>(sleep, delay, gettimeofday)</label>

# interrupt

1.  Interrupt Handling
2.  Exception Handling
3.  Softirq
4.  Tasklet
5.  Workqueue

# power

1.  PSCI Framework
2.  CPU Idle
3.  System Suspend <label>(suspend, resume, dpm, wakelock, wakeup source)</label>
4.  Runtime Suspend
5.  CPU Hotplug
6.  System Reboot
7.  CPU Freq
8.  DEV Freq

# amp

1.  firmware
2.  remoteproc
3.  mailbox
4.  rpmsg
5.  openamp

# cgroup

1.  cgroup v1
2.  cgroup v2

# kvm

1.  kvm
2.  vhe
3.  pkvm
4.  virtio

# miscellaneous

1.  printk
2.  syscall
3.  signal
4.  uevent
5.  netlink
6.  initcall
7.  bootconfig
8.  kallsyms
9.  irqwork
10.  wakeup <label>(wait queue / swait / rcuwait / completion)</label>

# drivers

1.  dma-engine

# tracing

1.  ftrace
2.  perf
3.  flamegraph
4.  bcc
5.  strace
6.  uftrace
7.  pstack
8.  bootchart

# benchmarks

1.  dhrystone
2.  coremark
3.  speccpu
4.  geekbench
5.  mlperf
6.  sysbench
7.  schbench
8.  rt-tests
9.  stress-ng
10. lmbench
11. stream
12. bandwidth
13. memtester
14. core-to-core-latency
15. dd
16. fio
17. iozone
18. dbench
19. iperf3

# build system

1. kbuild/kconfig
2. buildroot
3. yocto
4. debootstrap, multistrap
5. qemu-aarch64-static
6. distcc & icecream

# development

1.  Build Linux Kernel
2.  Run and Debug Kernel with QEMU & GDB <label>(lightbox)</label>
3.  Read Linux Source with Sublime
4.  Skills to Exporing the Linux Kernel
5.  Tracing Skills
