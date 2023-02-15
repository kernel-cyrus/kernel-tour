# cpufreq

CPUFreq框架是内核的CPU调频框架，整个调频框架由以下几个部分组成：

- cpufreq driver

- cpufreq framework (core)

- cpufreq governor

一些基本概念：

P-state: voltage and frequency point

总体上，CPU Freq主要在sysfs向userspace提供了调频操作和策略设置节点。当使用一些自动调频的governor时，允许governor从系统获取信息，自动触发调频动作。

另外，还向其他模块提供了qos频率投票，以及调频事件的notifier机制。

下面分开来看每个部分。

## Driver

Cpufreq Driver主要负责底层调频的操作，核心是实现 `cpufreq_dirver` 对象，并通过 `cpufreq_register_driver` 将驱动注册到系统，供core部分使用。

其中，最核心的几个回调包括：

- online: hotplug callback
- offline: hotplug callback
- target_index: set freq
- set_boost: enter into boost mode

以 `mediatek-cpufreq-hw.c` 为例，其freq table保存在硬件寄存器中，driver初始化时，从硬件中读出freq table，并实现相关调频回调，最终注册到core。

db845c使用的 `qcom-cpufreq-hw.c` 为例，其freq table保存在dts中，driver使用OPP框架来读取这些配置，并实现cpufreq回调。

\* OPP（Operating Performance Point，提供dts的freq-valt-table配置，及获取接口。

## Core

Core部分核心文件是 `/drivers/cpufreq/cpufreq.c`

这个文件主要实现了BOOST，DRIVER，SYSFS，FREQ，NOTIFIER，GOVERNOR，POLICY相关功能的操作接口。

`cpufreq_register_driver` (cpufreq_driver* cpufreq_driver)

`cpufreq_register_governor` (cpufreq_governor_list)

`cpufreq_register_notifier` (cpufreq_transition_notifier_list)

`cpufreq_policy_alloc` (cpufreq_policy* PERCPU:cpufreq_cpu_data)

`cpufreq_policy` 是顶层调频管理和操作的PERCPU对象，这个结构保存了每个CPU调频相关的所有私有信息，供各类操作接口使用。

Mainflow：

1、cpufreq driver注册

2、governor注册

3、cpu hotplug state: "cpufreq:online"，`cpuhp_cpufreq_online`、`cpuhp_cpufreq_offline`，创建出percpu policy，绑定driver、governor

4、向sysfs创建节点，提供功能

5、governor开始工作，通过 `__cpufreq_driver_target` 进行调频。

* Qos是调频投票机制，用来实现max_freq、min_freq limitation。(`cpufreq_notifier_min` `cpufreq_notifier_max`)，在其他module有需求投票后，freq constrants会被更新。

## Sysfs

`/sys/devices/system/cpu/cpu0/cpufreq/policyX`

```
scaling_driver			r	# cpufreq driver name
affected_cpus			r	# cpu
related_cpus			r	# <?>
scaling_cur_freq		rw	# cur freq
scaling_min_freq		rw	# min freq limit
scaling_max_freq		rw	# max freq limit
scaling_governor		rw	# selected governor
scaling_available_governors	r	# available governors
scaling_setspeed		r	# <not supported>
cpuinfo_min_freq		r	# hardware min freq
cpuinfo_max_freq		r	# hardware max freq
cpuinfo_transition_latency	r	# hardware latency
```

## Governors

Governor负责从系统收集信息，并触发调频动作。

**powersave**

always lowest frequency

**performance**

always highest frequency

**schedutil**

uses CPU utilization data from scheduler to determine target frequence

**userspace**

allow user space set the frequency

**ondemand**

uses CPU load as a CPU frequency selection metric

it reaches the target by CPU load, 0->min, 100->max

**conservative**

uses CPU load as a CPU frequency selection metric

it reaches the target step by step, up_threshold->up, down_threshold->down

## Files

```
- /drivers/cpufreq/cpufreq.c
- /drivers/cpufreq/cpufreq-dt.c
- /drivers/cpufreq/cpufreq_stats.c
- /drivers/cpufreq/cpufreq_conservative.c
- /drivers/cpufreq/cpufreq_governor.c
- /drivers/cpufreq/cpufreq_ondemand.c
- /drivers/cpufreq/cpufreq_powersave.c
- /drivers/cpufreq/cpufreq_performance.c
- /drivers/cpufreq/cpufreq_userspace.c
- /kernel/sched/cpufreq_schedutil.c
- /kernel/power/qos.c
```

## Reference

<https://www.kernel.org/doc/html/latest/admin-guide/pm/cpufreq.html>

<https://docs.kernel.org/scheduler/schedutil.html>

<http://www.wowotech.net/pm_subsystem/cpufreq_overview.html>

<http://www.wowotech.net/pm_subsystem/cpufreq_driver.html>

<http://www.wowotech.net/pm_subsystem/cpufreq_core.html>

<http://www.wowotech.net/pm_subsystem/cpufreq_governor.html>