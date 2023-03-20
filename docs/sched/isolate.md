# cpu isolation

CPU isolation可以通过简单的bootargs将CPU从调度器隔离出来，以创造一个没人打扰的进程运行环境。

每个隔离的功能（隔离的干净程度），都可以通过flag单独控制：

```
enum hk_flags {
	HK_FLAG_TIMER
	HK_FLAG_RCU
	HK_FLAG_MISC
	HK_FLAG_SCHED		# 禁用load balance
	HK_FLAG_TICK		# 禁用tick (nohz)
	HK_FLAG_DOMAIN
	HK_FLAG_WQ		# 禁止往isolcpu的unbound workqueue放置任务
	HK_FLAG_MANAGED_IRQ	# 禁止shared irq
	HK_FLAG_KTHREAD
};
```

其中，几个主要隔离功能的实现如下：

```
HK_TYPE_TICK
	sched_tick_start bypass
	sched_tick_stop bypass

HK_TYPE_SCHED
	nohz_balance_enter_idle
	nohz_newidle_balance

HK_TYPE_WQ
	workqueue_init_early(wq_unbound_cpumask)

HK_TYPE_DOMAIN
	sched_init_domains
```

CPU隔离后，可以用taskset来向隔离CPU放置任务

```
taskset -cp 0-3 <pid>
```

也可以和cpuset配合使用，为isolcpus创建taskset：

```
mkdir /sys/fs/cgroup/test
echo 0 > /sys/fs/cgroup/test/cpuset.mems
echo 0-3 > /sys/fs/cgroup/test/cpuset.cpus
```

但是这些taskset并不会负载均衡。

## Bootargs

```
普通隔离：isolcpus=
	housekeeping_isolcpus_setup
		housekeeping_setup
```

```
极致干净：nohz_full= （禁用tick）
	housekeeping_nohz_full_setup
		housekeeping_setup
			tick_nohz_full_setup(cpumask)
```

## 代码

`/kernel/sched/isolation.c`

```
housekeeping_setup
	housekeeping_mask -> 允许调度的核
	non_housekeeping_mask -> 隔离核（isolcpus）
	housekeeping -> 各种isol功能对应的cpumask
```

```
start_kernel			(main.c)
	housekeeping_init	(isolation.c)
```

## 参考

<https://lwn.net/Articles/816298/>

<https://lwn.net/Articles/270623/>

<https://www.suse.com/c/cpu-isolation-full-dynticks-part2/>