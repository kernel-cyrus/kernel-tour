### kernel

[initcall](/kernel/initcall)
<label>(module init sections)</label>

[params](/kernel/params)
<label>(bootargs and module params)</label>

[bootconfig](/kernel/bootconfig)
<label>(extends bootargs with bootconfig file)</label>

[sysctl](/kernel/sysctl)
<label>(sysctl command for procfs)</label>

[panic](/kernel/oops)
<label>(kernel oops format and how to trigger it)</label>

[kallsyms](/kernel/kallsyms)
<label>(runtime symbol table embeded in kernel image)</label>

[rwonce](/kernel/rwonce)
<label>(barriers to make sure the value is read from ddr)</label>

[kobject, ktype, kset, kref](/kernel/kobject)
<label>(kernel base objects)</label>

module
<label>(kernel module load flow)</label>

[printk](/kernel/printk)
<label>(printk and logs)</label>

[syscall](/kernel/syscall)
<label>(enter into kernel with system call)</label>

netlink
<label>(communication between userspace and kernel with socket)</label>

[uevent](/kernel/uevent)
<lable>(send event form kernel to userspce)</lable>

### locking

[spinlock](/locking/spinlock)
<label>(spin locks)</label>

[mutex](/locking/mutex)
<label>(locks that can sleep)</label>

[semaphore](/locking/semaphore)
<label>(just semaphore)</label>

[rwsem](/locking/rwsem)
<label>(optimize read/write case for semaphore)</label>

[rcu](/locking/rcu)
<label>(read-write-update mechanism to optimize rw locking)</label>

futex
<label>(locks for userspace)</label>

[lockdep](/locking/lockdep)
<label>(dead lock detection)</label>

[lockstat](/locking/lockstat)
<label>(locks statistics)</label>

### irq

[irqdomain](/irq/irqdomain)
<label>(irq controller to create hwirq-virq mapping)</label>

[irqdesc](/irq/irqdesc)
<label>(an virq descriptor)</label>

[irqchip](/irq/irqchip)
<label>(interrupt controller device, like gic driver)</label>

[manage](/irq/manage)
<label>(operate an virq)</label>

[exception, fault, traps](/irq/exception)
<label>(exception handling)</label>

[procfs, debugfs, sysfs](/irq/sysfs)
<label>(interrupt related filesystem nodes)</label>

[softirq](/irq/softirq)
<label>(bottom half: softirq handled with irq enable)</label>

[tasklet](/irq/tasklet)
<label>(bottom half: register bh handler to softirq)</label>

[workqueue](/irq/workqueue)
<label>(bottom half: workqueue thread to handle bh works)</label>

### time

[arch_timer](/time/arch_timer)
<label>(arm arch timer device driver)</label>

[clocksource](/time/clocksource)
<label>(the backend clock source of timekeeping system)</label>

[timekeeping](/time/timekeeping)
<label>(various clocks to provide timestamps)</label>

[clockevents](/time/clockevents)
<label>(a device to gen hw timer interrupt)</label>

[tickdevice](/time/tickdevice)
<label>(a device to gen ticks)</label>

[jiffies](/time/jiffies)
<label>(a tick counter)</label>

[timer](/time/timer)
<label>(legacy soft timer)</label>

[hrtimer](/time/hrtimer)
<label>(hi-res soft timer)</label>

[tickless](/time/tickless)
<label>(disable ticks while we don't have any work to do)</label>

tickbroadcast

[delay](/time/delay)
<label>(delay/sleep functions)</label>

[timer_list](/time/timer_list)
<label>(timer sysfs nodes and formats)</label>

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
<label>(page table data structures)</label>

[mmu](/mm/mmu)
<label>(page table creation)</label>

[pagewalk, ptdump](/mm/pagewalk)
<label>(interfaces to walk through or dump the page table)</label>

[fixmap](/mm/fixmap)
<label>(fixed va-pa mapping)</label>

[convert](/mm/convert)
<label>(va-pa-pfn-page transformation)</label>

[memblock](/mm/memblock)
<label>(physical memory management)</label>

[tlb](/mm/tlb)
<label>(translation buffer operations)</label>

[percpu](/mm/percpu)
<label>(declare private variables for each cpu)</label>

### power

[psci](/power/psci)
<label>(put low level power operations into el2/el3)</label>

[cpuidle](/power/cpuidle)
<label>(run cpu into idle)</label>

[suspend](/power/suspend)
<label>(system suspend)</label>

[dpm](/power/dpm)
<label>(device power management)</label>

[runtime_suspend](/power/runtime)
<label>(put device into low-power mode while not using)</label>

domain
<label>power domain management</label>

[wakelock](/power/wakelock)
<label>(locks to prevent system suspend)</label>

[wakeup](/power/wakeup)
<label>(wakeup source irq)</label>

[spin_table](/power/spin_table)
<label>(an implementation to boot smp)</label>

[smp](/power/smp)
<label>(smp boot flow)</label>

[cpu_hotplug](/power/cpuhp)
<label>(runtime bringup or teardown cpus)</label>

[rebooot](/power/reboot)
<label>(system reboot and poweroff)</label>

[cpufreq](/power/cpufreq)
<label>(cpu dvfs framework and soc driver)</label>

devfreq
<label>(device dvfs framework)</label>

energy_model

### process

[state](/process/state)
<label>(process states)</label>

fork

### sched

[arch](/sched/arch)
<label>(from the very begining of scheduling)</label>

stop
<label>(stop schedule class)</label>

deadline
<label>(deadline schedule class)</label>

rt
<label>(rt schedule class)</label>

fair
<label>(cfs schedule class)</label>

idle
<label>(idle schedule class)</label>

[pelt](/sched/pelt)
<label>(per-entity load tracking)</label>

walt
<label>(window-asist load tracking)</label>

sched_util
<label>(cpufreq governor)</label>

load_balance
<label>load balancing</label>

eas
<label>(energy aware scheduling)</label>

task_group
<label>(task group scheduling)</label>

isolate

completion

psi

### cgroup

cgroup

cpuset

memcontrol

### fs

[filesystem](/fs/filesystem)
<label>(register a new filesystem)</label>

[super](/fs/super)
<label>(a superblock object represents a mounted filesystem)</label>

[inode](/fs/inode)
<label>(inode is a file or directory of a filesystem)</label>

[dentry](/fs/dentry)
<label>(path object)</label>

[filemode](/fs/filemode)
<label>(file permissions)</label>

[ramfs, tmpfs](/fs/ramfs)
<label>(fs live in ram)</label>

[devtmpfs](/fs/devtmpfs)
<label>(/dev)</label>

[procfs](/fs/procfs)
<label>(/proc)</label>

[debugfs](/fs/debugfs)
<label>(/sys/kernel/debug)</label>

[sysfs](/fs/sysfs)
<label>(/sys)</label>

initramfs
<label>(rootfs)</label>

fat32

ext4

erofs

f2fs

fuse

### block

[zram](/block/zram)
<label>(ram based block device that can auto compressed)</label>

ramdisk
<label>(block device based on physical memory)</label>

### drivers

driver
<label>(device driver and probe, async probe)

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
<label>(sprintf interfaces)</label>

[string](/lib/string)
<label>(string interfaces in kernel)</label>

[debugobject](/lib/debugobject)
<label>(object life cycle management)</label>

### android

[vendor_hook](/android/vendor_hook)
<label>(hooks based on tracepoint for ko to inject code into kernel)</label>

### tracing

[tracepoint](/tracing/tracepoint)
<label>(kernel hooks)</label>

[trace_event](/tracing/trace_event)
<label>(trace event based on tracepoint)</label>

[ftrace](/tracing/ftrace)
<label>(function trace)</label>

kprobe

fprobe

uprobe

[perf](/tracing/perf)
<label>(linux performance tool)</label>

ebpf

coresight

### tools

perfetto

kernelshark

flamechart

simpleperf

### scripts

...