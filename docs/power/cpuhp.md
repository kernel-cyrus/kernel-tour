# cpu hotplug

CPU hotplug是多核系统的CPU动态插拔核机制。

开启 `CONFIG_HOTPLUG_CPU` 后，系统会提供出CPU的操作接口：

```
 $ ls -lh /sys/devices/system/cpu
 total 0
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu0
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu1
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu2
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu3
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu4
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu5
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu6
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu7
 drwxr-xr-x  2 root root    0 Dec 21 16:33 hotplug
 -r--r--r--  1 root root 4.0K Dec 21 16:33 offline	# cpu mask
 -r--r--r--  1 root root 4.0K Dec 21 16:33 online	# cpu mask
 -r--r--r--  1 root root 4.0K Dec 21 16:33 possible	# cpu mask
 -r--r--r--  1 root root 4.0K Dec 21 16:33 present	# cpu mask
```

在各个cpu节点下，可以通过online节点进行插拔核：

```
 $ echo 0 > /sys/devices/system/cpu/cpu4/online
 smpboot: CPU 4 is now offline
 $ echo 1 > /sys/devices/system/cpu/cpu4/online
 smpboot: Booting Node 0 Processor 4 APIC 0x1
```

每个CPU的hotplug状态，保存在 `struct cpuhp_cpu_state cpuhp_state` 中。

## CPU Mask

CPU Mask定义了cpumask_t用来表示cpu的bitmap。

```
- /include/linux/cpumask.h	# cpumask_t, global variable and operations
- /drivers/base/cpu.c		# cpumask sysfs interface
```

主要的bitmap有以下几个：

cpu_online_mask：online CPU

cpu_present_mask：online CPU + offline CPU

cpu_possible_mask：online CPU + offline CPU + additional CPU

内核中使用这些mask，可以使用cpumask.h中定义的接口，对mask的bit进行设置，或者使用for_each_cpu / for_each_xxx_cpu 来对某个mask的cpu进行遍历。

sysfs中提供了这些mask的获取和设置节点:

`/sys/devices/system/cpu/online`

`/sys/devices/system/cpu/offline`

`/sys/devices/system/cpu/present`

`/sys/devices/system/cpu/possible`

## CPU Hotplug State

CPU的插拔过程是一个链路，链路中的每一环，定义为一个 `cpuhp_step`

```
struct cpuhp_step
	name		# state name
	startup		# up function
	teardown	# down function
```

这个长长的step链路，组成了全部的cpu hp state，所有state定义在 `cpuhp_state`（cpuhotplug.h） 中，所有step instance保存在 `cpuhp_hp_states` 数组中。

sysfs提供了以下节点来显示这个cpu hotplug state数组：

`/sys/devices/system/cpu/hotplug/states`

整个链路上包括三个比较关键的state：

1. PREPARE，CPUHP_OFFLINE--CPUHP_BRINGUP_CPU

2. STARTING，CPUHP_BRINGUP_CPU+1--CPUHP_AP_ONLINE

3. ONLINE，CPUHP_AP_ONLINE+1--CPUHP_ONLINE

CPU从一个state切换到另一个state，会遍历并执行链路中的每一个state的对应startup或者teardown函数。

CPU插拔过程中的所有具体操作，都基于这个链路框架实现，比如下面几个比较重要的step：

**CPU最终上下电**

- CPUHP_BRINGUP_CPU：`bringup_cpu` -> `__cpu_up` --> `cpu_ops->cpu_boot`

- CPUHP_TEARDOWN_CPU： `takedown_cpu` -> `__cpu_die` --> `cpu_ops->cpu_kill`

最终操作PSCI完成。

有两种方式为state注册回调：

1、静态方式：

在 `cpuhp_state` 中定义新的state，在 `cpuhp_hp_states` 中添加state对应的callbacks。

2、动态方式：

在 `cpuhp_state` 中定义新的state，调用 `cpuhp_setup_state` 为state注册回调。

## CPU Hotplug Trigger

`_cpu_up`、`_cpu_down` 是CPU Hotplug核心执行函数。

这两个函数会操作CPU到online或offline，并执行state链路中的所有callbacks。

有以下四种情况可以触发CPU Hotplug动作。

**1、Secondary CPU Boot**

在smp启动阶段，`_cpu_up` 会执行，也就说，smp在启动阶段的初始化，复用了hotplug的过程。

**2、Sysfs Online Node**

`cpu_subsys` bus为CPU提供了online操作节点：

`/sys/devices/system/cpu/cpuX/online`

显示CPU是否online，写0 offline，写1 online。

相关文件： `/drivers/base/cpu.c`

**3、Sysfs Target Node**

`/sys/devices/system/cpu/cpuX/hotplug/state`

当前CPU的hotplug state，read only。

`/sys/devices/system/cpu/cpuX/hotplug/target`

当前CPU的hotplug state，写state id触发hotplug的up或者down流程，到对应的state。

`/sys/devices/system/cpu/cpuX/hotplug/fail`

显示上次失败的state。

相关文件： `/kernel/cpu.c`

**4、Suspend**

`suspend_enter`

调用 `suspend_disable_secondary_cpus` / `suspend_enable_secondary_cpus`

最终执行 `_cpu_up` / `_cpu_down`，完成Secondary CPU的插拔。

## CPU Hotplug Thread

用于创建percpu绑核thread，在cpu hotplug过程中，这些thread会被置为park状态，并在CPU重新online后置为unpark。

park类似freeze，是绑核线程的一种暂停状态。

`struct smp_hotplug_thread`

hotplug thread结构体

`smpboot_register_percpu_thread`

注册percpu hotplug thread

`smpboot_unregister_percpu_thread`

移除percpu hotplug thread

`smpboot_thread_fn`

hotplug thread loop func，执行时会判断td->thread_should_run，如果条件满足，则运行td->thread_fn，然后进入Sleep Interruptable状态，等待下次唤醒执行。

相关文件：`/kernel/smpboot.c`

## CPU Idle vs Suspend vs Runtim Suspend vs CPU Hotplug

**CPU Idle**

CPU空闲，会根据governor决定进入哪个CPU Idle State，底层由PSCI实现

如：C0 WFI，C1 Core Retention，C2 Core Power Down，C3 Cluster Power Down

Idle不会进Suspend，只要有新的任务或中断到来，就会出Idle状态。

**Suspend**

系统只有通过写入文件节点（类似点击休眠或者合盖），来手动操作进入Suspend。

Suspend是全系统的深度休眠，CPU、Device会Suspend下电，CPU下电过程中会通过CPU Hotplug对Secondary CPU下电。

Suspend时，所有在运行的Process都进入Freeze状态暂停运行。

Suspend后，只有唤醒源中断可以唤醒。

**Device Runtime Suspend**

Device空闲时，会自动进入Device Runtime Suspend

**CPU Hotplug**

插拔核同样由手动触发，或者在Suspend过程中执行。

teardown时，会执行Process会进行核迁移，绑核的Thread会进入Park状态。

## Reference

<https://docs.kernel.org/core-api/cpu_hotplug.html>

<https://zhuanlan.zhihu.com/p/545550388>

<https://www.cnblogs.com/LoyenWang/p/11397084.html>