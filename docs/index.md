<style>
label {
    margin-left: 15px;
    font-size: 14px;
    color: gray;
}
</style>

### kernel

[initcall](/kernel/initcall)

[params](/kernel/params)

[panic](/kernel/oops)

[kallsyms](/kernel/kallsyms)

[rwonce](/kernel/rwonce)

[kobject, ktype, kset, kref](/kernel/kobject)

module
<label>(kernel module load flow)</label>

driver
<label>(device driver and probe, async probe)

printk
<label>(console and printk)</label>

### locking

[spinlock](/locking/spinlock)

[mutex](/locking/mutex)

[semaphore](/locking/semaphore)

[rwsem](/locking/rwsem)

[rcu](/locking/rcu)

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
<label>(dma device driver framework)</label>

[coherent](/dma/coherent)
<label>(for non-coherent device: global dma pool & device dma pool)</label>

[contiguous](/dma/contiguous)
<label>(global cma pool & device cma pool)</label>

[atomic pool](/dma/pool)
<label>(dynamic global dma pool)</label>

[direct](/dma/direct)
<label>(for no iommu, direct_alloc)</label>

[dma-mapping](/dma/dma-mapping)
<label>(main dma allocation interface)</label>

[dma-pool](/dma/dma-pool)
<label>(create a pool for small dma buffers)</label>

[dma-heap](/dma/dma-heap)
<label>(framework for userspace to alloc dma-buf form kernel dma pools)</label>

[dma-buf](/dma/dma-buf)
<label>(dma buffer management)</label>

### iommu

[iommu](/iommu/iommu)
<label>(iommu framework)</label>

[smmu](/iommu/smmu)
<label>(arm smmu device driver)</label>

### kvm

[kvm](/kvm/kvm)
<label>(kernel virtual machine and it's arm implementation)</label>

[vhe](/kvm/vhe)
<label>(move kernel and hyp to el2 to optimize vm performance)</label>

[pkvm](/kvm/pkvm)
<label>(new security model based on kvm for android)</label>

### virtio

[virtio](/virtio/virtio)
<label>(virtio device, driver and virtio bus, matched by virtio device id)</label>

[virtio_ring](/virtio/virtio_ring)
<label>(virtio message queue between front and backend driver)</label>

[virtio_mmio](/virtio/virtio_mmio)
<label>(create virtio device which is based on mmio)</label>

### rproc

[firmware](/rproc/firmware)
<label>(firmware image reader)</label>

[remoteproc](/rprc/remoteproc)
<label>(remote processor lifecycle management)</label>

[rpmsg](/rproc/rpmsg)
<label>(remote processor communication)</label>

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

[psci](/power/psci)

[cpuidle](/power/cpuidle)

[suspend](/power/suspend)

[dpm](/power/dpm)

[runtime_suspend](/power/runtime)

[wakelock](/power/wakelock)

[wakeup](/power/wakeup)

[spin_table](/power/spin_table)

[smp](/power/smp)

[cpu_hotplug](/power/cpuhp)

domain

poweroff

reset

cpufreq

devfreq

energy_model

qos

### process

[state](/process/state)

fork

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

completion

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

fat32

ext4

erofs

f2fs

fuse

### block

[zram](/block/zram)

ramdisk

### drivers

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

uart

input

### graphics

framebuffer

drm/kms

### libs

[sprintf](/lib/sprintf)

[string](/lib/string)

[debugobject](/lib/debugobject)

### android

[vendor_hook](/android/vendor_hook)

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