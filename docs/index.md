<style>
.markdown-body p {
    position: relative;
}

label {
    position: absolute;
    padding-top: 2px;
    left: 150px;
    font-size: 14px;
    color: gray;
}
</style>

### init

[initcall](/init/initcall)

[params](/init/params)

### kernel

[oops, panic](/kernel/oops)

[kallsyms, stacktrace](/kernel/kallsyms)

[rwonce](/kernel/rwonce)

[rcu](/kernel/rcu)

### locking

[spinlock](/locking/spinlock)

[mutex](/locking/mutex)

[semaphore](/locking/semaphore)

[rwsem](/locking/rwsem)

[lockdep](/locking/lockdep)

[lockstat](/locking/lockstat)

### irq

[irqdomain](/irq/irqdomain)

[irqdesc](/irq/irqdesc)

[irqchip](/irq/irqchip)

[manage](/irq/manage)

[exception, fault, traps](/irq/exception)

[procfs, debugfs, sysfs](/irq/sysfs)

[softirq](/irq/softirq)

[tasklet](/irq/tasklet)

[workqueue](/irq/workqueue)

### time

[arch_timer](/time/arch_timer)

[clocksource](/time/clocksource)

[timekeeping](/time/timekeeping)

[clockevents](/time/clockevents)

[tickdevice](/time/tickdevice)

[jiffies](/time/jiffies)

[timer](/time/timer)

[hrtimer](/time/hrtimer)

[tickless](/time/tickless)

tickbroadcast

[delay](/time/delay)

[timer_list](/time/timer_list)

### dma

[dma-engine](/dma/dma-engine)

[coherent](/dma/coherent)
<label>for non-coherent device: global dma pool & device dma pool</label>

[contiguous](/dma/contiguous)
<label>global cma pool & device cma pool</label>

[atomic pool](/dma/pool)
<label>dynamic global dma pool</label>

[direct](/dma/direct)
<label>for no iommu, direct_alloc</label>

[dma-mapping](/dma/dma-mapping)
<label>main dma allocation interface</label>

[dma-pool](/dma/dma-pool)
<label>create a pool for small dma buffers</label>

[dma-heap](/dma/dma-heap)
<label>framework for userspace to alloc dma-buf form kernel dma pools</label>

[dma-buf](/dma/dma-buf)

### iommu

[iommu](/iommu/iommu)

[smmu](/iommu/smmu)

### virtio

...

### remoteproc

...

### rpmsg

...

### kvm

[kvm](/kvm/kvm)
<label>kernel virtual machine and it's arm implementation</label>

[vhe](/kvm/vhe)
<label></label>

[pkvm](/kvm/pkvm)
<label></label>

### mm

[pgtable](/mm/pgtable)

[mmu](/mm/mmu)

[pagewalk, ptdump](/mm/pagewalk)

[fixmap](/mm/fixmap)

[convert](/mm/convert)

[memblock](/mm/memblock)

[tlb](/mm/tlb)

[percpu](/mm/percpu)

### power

cpuidle

cpufreq

devfreq

energy_model

qos

suspend

reset

### process

[state](/process/state)

### sched

stop

deadline

rt

fair

idle

cpufreq

pelt / walt

eas

isolate

### cgroup

cgroup

cpuset

memcontrol

### fs

[filesystem](/fs/filesystem)

[super](/fs/super)

[inode](/fs/inode)

[dentry](/fs/dentry)

[filemode](/fs/filemode)

[ramfs, tmpfs](/fs/ramfs)

[devtmpfs](/fs/devtmpfs)

[procfs](/fs/procfs)

[debugfs](/fs/debugfs)

[sysfs](/fs/sysfs)

### device

[kobject, ktype, kset, kref](/device/kobject)

### block

[zram](/block/zram)

### lib

[sprintf](/lib/sprintf)

[string](/lib/string)

[debugobject](/lib/debugobject)

### tracing

[tracepoint](/tracing/tracepoint)

[trace_event](/tracing/trace_event)

[ftrace](/tracing/ftrace)

kprobe

fprobe

uprobe

perf

ebpf

coresight

### tools

perfetto

kernelshark

flamechart

simpleperf