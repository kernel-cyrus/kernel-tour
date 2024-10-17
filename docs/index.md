### kernel

[initcall](/archives/kernel/initcall)
<label>(module init sections)</label>

[params](/archives/kernel/params)
<label>(bootargs and module params)</label>

[bootconfig](/archives/kernel/bootconfig)
<label>(extends bootargs with bootconfig file)</label>

[sysctl](/archives/kernel/sysctl)
<label>(sysctl command for procfs)</label>

[panic](/archives/kernel/oops)
<label>(kernel oops format and how to trigger it)</label>

[kallsyms](/archives/kernel/kallsyms)
<label>(runtime symbol table embeded in kernel image)</label>

[rwonce](/archives/kernel/rwonce)
<label>(barriers to make sure the value is read from ddr)</label>

[kobject, ktype, kset, kref](/archives/kernel/kobject)
<label>(kernel base objects)</label>

module
<label>(kernel module load flow)</label>

[printk](/archives/kernel/printk)
<label>(printk and logs)</label>

[syscall](/archives/kernel/syscall)
<label>(enter into kernel with system call)</label>

[netlink](/archives/kernel/netlink)
<label>(communication between userspace and kernel with socket)</label>

[uevent](/archives/kernel/uevent)
<label>(send event form kernel to userspce)</label>

[irqwork](/archives/kernel/irqwork)
<label>(enqueueing and running callbacks in hardirq context)</label>

### locking

[spinlock](/archives/locking/spinlock)
<label>(spin locks)</label>

[mutex](/archives/locking/mutex)
<label>(locks that can sleep)</label>

[semaphore](/archives/locking/semaphore)
<label>(just semaphore)</label>

[rwsem](/archives/locking/rwsem)
<label>(optimize read/write case for semaphore)</label>

[rcu](/archives/locking/rcu)
<label>(read-write-update mechanism to optimize rw locking)</label>

[futex](/archives/locking/futex)
<label>(basic lock for userspace)</label>

[lockdep](/archives/locking/lockdep)
<label>(dead lock detection)</label>

[lockstat](/archives/locking/lockstat)
<label>(locks statistics)</label>

### irq

[irqdomain](/archives/irq/irqdomain)
<label>(irq controller to create hwirq-virq mapping)</label>

[irqdesc](/archives/irq/irqdesc)
<label>(an virq descriptor)</label>

[irqchip](/archives/irq/irqchip)
<label>(interrupt controller device, like gic driver)</label>

[manage](/archives/irq/manage)
<label>(operate an virq)</label>

[exception, fault, traps](/archives/irq/exception)
<label>(exception handling)</label>

[procfs, debugfs, sysfs](/archives/irq/sysfs)
<label>(interrupt related filesystem nodes)</label>

[softirq](/archives/irq/softirq)
<label>(bottom half: softirq handled with irq enable)</label>

[tasklet](/archives/irq/tasklet)
<label>(bottom half: register bh handler to softirq)</label>

[workqueue](/archives/irq/workqueue)
<label>(bottom half: workqueue thread to handle bh works)</label>

### time

[arch_timer](/archives/time/arch_timer)
<label>(arm arch timer device driver)</label>

[clocksource](/archives/time/clocksource)
<label>(the backend clock source of timekeeping system)</label>

[timekeeping](/archives/time/timekeeping)
<label>(various clocks to provide timestamps)</label>

[clockevents](/archives/time/clockevents)
<label>(a device to gen hw timer interrupt)</label>

[tickdevice](/archives/time/tickdevice)
<label>(a device to gen ticks)</label>

[jiffies](/archives/time/jiffies)
<label>(a tick counter)</label>

[timer](/archives/time/timer)
<label>(legacy soft timer)</label>

[hrtimer](/archives/time/hrtimer)
<label>(hi-res soft timer)</label>

[tickless](/archives/time/tickless)
<label>(disable ticks while we don't have any work to do)</label>

tickbroadcast

[delay](/archives/time/delay)
<label>(delay/sleep functions)</label>

[timer_list](/archives/time/timer_list)
<label>(timer sysfs nodes and formats)</label>

### dma

[dma-engine](/archives/dma/dma-engine)
<label>(dma device driver framework)</label>

[coherent](/archives/dma/coherent)
<label>(for non-coherent device: global dma pool & device dma pool)</label>

[contiguous](/archives/dma/contiguous)
<label>(global cma pool & device cma pool)</label>

[atomic pool](/archives/dma/pool)
<label>(dynamic global dma pool)</label>

[direct](/archives/dma/direct)
<label>(for no iommu, direct_alloc)</label>

[dma-mapping](/archives/dma/dma-mapping)
<label>(main dma allocation interface)</label>

[dma-pool](/archives/dma/dma-pool)
<label>(create a pool for small dma buffers)</label>

[dma-heap](/archives/dma/dma-heap)
<label>(framework for userspace to alloc dma-buf form kernel dma pools)</label>

[dma-buf](/archives/dma/dma-buf)
<label>(dma buffer management)</label>

### iommu

[iommu](/archives/iommu/iommu)
<label>(iommu framework)</label>

[smmu](/archives/iommu/smmu)
<label>(arm smmu device driver)</label>

### kvm

[kvm](/archives/kvm/kvm)
<label>(kernel virtual machine and it's arm implementation)</label>

[vhe](/archives/kvm/vhe)
<label>(move kernel and hyp to el2 to optimize vm performance)</label>

[pkvm](/archives/kvm/pkvm)
<label>(new security model based on kvm for android)</label>

### virtio

[virtio](/archives/virtio/virtio)
<label>(virtio device, driver and virtio bus, matched by virtio device id)</label>

[virtio_ring](/archives/virtio/virtio_ring)
<label>(virtio message queue between front and backend driver)</label>

[virtio_mmio](/archives/virtio/virtio_mmio)
<label>(create virtio device which is based on mmio)</label>

### rproc

[firmware](/archives/rproc/firmware)
<label>(firmware image reader)</label>

[remoteproc](/archives/rproc/remoteproc)
<label>(remote processor lifecycle management)</label>

[rpmsg](/archives/rproc/rpmsg)
<label>(remote processor communication)</label>

### mm

[overview](/archives/mm/overview)
<label>(memory management overview)</label>

[pgtable](/archives/mm/pgtable)
<label>(page table data structures)</label>

[mmu](/archives/mm/mmu)
<label>(page table creation)</label>

[pagewalk, ptdump](/archives/mm/pagewalk)
<label>(interfaces to walk through or dump the page table)</label>

[fixmap](/archives/mm/fixmap)
<label>(fixed va-pa mapping)</label>

[convert](/archives/mm/convert)
<label>(va-pa-pfn-page transformation)</label>

[memblock](/archives/mm/memblock)
<label>(physical memory management)</label>

[tlb](/archives/mm/tlb)
<label>(translation buffer operations)</label>

[percpu](/archives/mm/percpu)
<label>(declare private variables for each cpu)</label>

page allocate
<label>(buddy, slab)</label>

vmalloc
<label>(virtual memory management)</label>

mmap
<label>(mmap, kmap, vmap, ioremap)</label>

page swap / reclaim
<label>(kswapd, compaction, oom_killer)</label>

### power

[psci](/archives/power/psci)
<label>(put low level power operations into el2/el3)</label>

[cpuidle](/archives/power/cpuidle)
<label>(run cpu into idle)</label>

[suspend](/archives/power/suspend)
<label>(system suspend)</label>

[dpm](/archives/power/dpm)
<label>(device power management)</label>

[runtime_suspend](/archives/power/runtime)
<label>(put device into low-power mode while not using)</label>

domain
<label>power domain management</label>

[wakelock](/archives/power/wakelock)
<label>(locks to prevent system suspend)</label>

[wakeup](/archives/power/wakeup)
<label>(wakeup source irq)</label>

[spin_table](/archives/power/spin_table)
<label>(an implementation to boot smp)</label>

[smp](/archives/power/smp)
<label>(smp boot flow)</label>

[cpu_hotplug](/archives/power/cpuhp)
<label>(runtime bringup or teardown cpus)</label>

[rebooot](/archives/power/reboot)
<label>(system reboot and poweroff)</label>

[cpufreq](/archives/power/cpufreq)
<label>(cpu dvfs framework and soc driver)</label>

devfreq
<label>(device dvfs framework)</label>

energy_model

### process

[state](/archives/process/state)
<label>(process states)</label>

fork

### sched

[overview](/archives/sched/overview)
<label>(from the very begining of scheduling)</label>

[topology](/archives/sched/topology)
<label>(cpu topology, sched domain, sched groups)</label>

[stop](/archives/sched/stop)
<label>(stop schedule class and stop machine)</label>

[dl](/archives/sched/dl)
<label>(deadline schedule class)</label>

[rt](/archives/sched/rt)
<label>(rt schedule class)</label>

[fair](/archives/sched/fair)
<label>(cfs schedule class)</label>

[idle](/archives/sched/idle)
<label>(idle schedule class)</label>

[balance](/archives/sched/balance)
<label>cases of load balancing</label>

[pelt](/archives/sched/pelt)
<label>(per-entity load tracking)</label>

walt
<label>(window-asist load tracking)</label>

[schedutil](/archives/sched/schedutil)
<label>(cpufreq governor)</label>

[eas](/archives/sched/eas)
<label>(energy aware scheduling)</label>

[isolate](/archives/sched/isolate)
<label>(cpu isolation)</label>

[completion](/archives/sched/completion)
<label>(multi-core excution synchronize mechanism)</label>

psi

### cgroup

cgroup

cpuset

memcontrol

### fs

[filesystem](/archives/fs/filesystem)
<label>(register a new filesystem)</label>

[super](/archives/fs/super)
<label>(a superblock object represents a mounted filesystem)</label>

[inode](/archives/fs/inode)
<label>(inode is a file or directory of a filesystem)</label>

[dentry](/archives/fs/dentry)
<label>(path object)</label>

[filemode](/archives/fs/filemode)
<label>(file permissions)</label>

[ramfs, tmpfs](/archives/fs/ramfs)
<label>(fs live in ram)</label>

[devtmpfs](/archives/fs/devtmpfs)
<label>(/dev)</label>

[procfs](/archives/fs/procfs)
<label>(/proc)</label>

[debugfs](/archives/fs/debugfs)
<label>(/sys/kernel/debug)</label>

configfs

[sysfs](/archives/fs/sysfs)
<label>(/sys)</label>

initramfs
<label>(rootfs)</label>

fat32

ext4

erofs

f2fs

fuse

### block

[zram](/archives/block/zram)
<label>(ram based block device that can auto compressed)</label>

ramdisk
<label>(block device based on physical memory)</label>

### drivers

driver
<label>(device driver and probe, async probe)</label>

clock

dma

mailbox

hw-spinlock

pinctrl

gpio

reset

wdt

regulator

rtc

timers

tty/uart

input

### graphics

framebuffer

drm/kms

### libs

[sprintf](/archives/lib/sprintf)
<label>(sprintf interfaces)</label>

[string](/archives/lib/string)
<label>(string interfaces in kernel)</label>

[debugobject](/archives/lib/debugobject)
<label>(object life cycle management)</label>

### android

[vendor_hook](/archives/android/vendor_hook)
<label>(hooks based on tracepoint for ko to inject code into kernel)</label>

binder

kinfo

### tracing

[tracepoint](/archives/tracing/tracepoint)
<label>(kernel hooks)</label>

[trace_event](/archives/tracing/trace_event)
<label>(trace event based on tracepoint)</label>

[ftrace](/archives/tracing/ftrace)
<label>(function trace)</label>

kprobe

fprobe

uprobe

[perf](/archives/tracing/perf)
<label>(linux performance tool)</label>

ebpf

coresight

streamline

### tools

trace-cmd

kernelshark

perfetto

simpleperf

flamechart

atrace

strace

### benchmarks

hackbench

tbench

netperf

unixbench

reaim

schbench

stream

rt-app

sysbench

### other

linux system tools

binutils collection

<hr>

### My other projects

[tracecat](https://tracecat.kernel-tour.org/): A generic kernel trace analysis framework

[lightbox](https://lightbox.kernel-tour.org/): Arch64 qemu based kernel running and debug environment
