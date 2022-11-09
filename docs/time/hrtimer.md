# hrtime

与低精度timer不同，hrtimer实现了timer的精准到期控制。

hrtimer定义了8组timer base，hrtimer添加时可以使用不同时间类型做定时。

hrtimer在注册时使用的ktime可以选择使用ABS绝对时间，也可以使用基于now的REL相对时间做定时。

hrtimer提供了两种timer执行上下文：hard irq、soft irq

与低精度timer使用的哈希链表不同，hrtimer内部使用红黑树（timerqueue）来管理timer队列，hrtimer机制用自己的tick中断回调替换了原有的perioidic tick，原有的tick被实现为一个hrtimer(sched timer)注册到hrtimer队列，这样hrtimer实现了timer的精准到期控制，同时将tick纳入到了自己的timer loop中。

hrtimer的使能需要系统满足一些条件，正常情况下，在arch timer初始化结束后，会从low res切换到hres，同时替换tick中断处理函数，由hres完全接管tick。原来的tick功能由一个新的hrtimer（sched timer）来实现。

hrtimer也实现了debug object，可以对用户申请释放过程做预防监控。

`tick_switch_to_oneshot`

## Files

```
- /include/linux/hrtimer.h		# interface
- /kernel/time/hrtimer.c		# implementation
- /include/linux/timerqueue.h		# timer rb-tree
```

## Structures

`struct hrtimer`

hrtimer基本结构体，一个hrtimer，就是一个timer

主要定义回调函数、到期ktime、所使用的clock base、ktime是否为相对时间、硬timer还是软timer

`struct hrtimer_cpu_base`

per cpu hrtimer base，里面主要包括了一个clock base的合集。

`struct hrtimer_clock_base`

一个clock base，主要包括了这个clock base的get_time接口，timerqueue（所有使用这个clock base的timer队列）

`struct timerqueue...`

管理hrtimer的红黑树，用于降低hrtimer“队列”的插入和查找复杂度。

所以，综上：
- 一个cpu，有一个hrtimer_cpu_base
- 一个hrtimer_cpu_base，有一组cock base
- 每个clock base里，都有一个timerqueue
- 所有使用该clock的hrtimer，按顺序排列在timerqueue中

## Variables

`hrtimer_bases`

per cpu hrtimer_cpu_base

## Interfaces

`hrtimer_init`

初始化一个hrtimer

clock：CLOCK_REALTIME / CLOCK_MONOTONIC，会最终对应到对应的timer base上。

mode：HRTIMER_ABS / HRTIMER_REL设置ktime时间所表示的是wall time还是基于now的相对时间。

`hrtimer_start`

启动timer

`hrtimer_forward`

置后一个timer

`hrtimer_cancel`

取消一个timer，死等

`hrtimer_try_to_cancel`

取消一个timer，不死等

`hrtimer_restart`

取消的timer可以重新restart

`hrtimer_get_remaining`

获取timer剩余到期时间

`rtimer_active`

获取timer是否active

hrtimer_interrupt

## Functions

`hrtimers_init`

在start_kernel中初始化hrtimer机制，使能hrtimer softirq。

`hrtimer_run_softirq`

hrtimer软中断回调函数，用于运行HRTIMER_ACTIVE_SOFT类型的timer

```
hrtimer_run_softirq

	# 运行所有到期softirq timer（HRTIMER_ACTIVE_SOFT）
	__hrtimer_run_queues

	# 重新更新timerqueue，更新下次到期时间
	hrtimer_update_softirq_timer
		
		# 调用tick_program_event更新下次tick时间
		__hrtimer_reprogram
```

`hrtimer_run_queues`

hrtimer硬中断回调函数，用于运行HRTIMER_ACTIVE_HARD类型的timer

hres切换前，在tick device的tick中断回调中运行（run_local_timers）

hres切换后，HRTIMER_ACTIVE_HARD类型的timer在hrtimer机制自己的interrupt handler `hrtimer_interrupt` 中调用。

**low res => hres**

这个函数在legacy tick中还有个作用，就是不断判断是否能切换hres，在arch timer初始化结束后，切换条件满足，从legacy切换到hres。

```
tick_handle_periodic->
    run_local_timers
        hrtimer_run_queues
            hrtimer_switch_to_hres
                tick_init_highres
                - tick_switch_to_oneshot(hrtimer_interrupt)
                - tick_setup_sched_timer # add sched_timer
```

切换后，会用tick_setup_sched_timer创建新的tick timer（tick_sched_timer），替换原来的periodic_handler tick中断回调。后面jiffies也是在这个timer中更新

```
tick_sched_timer		# new sched tick handler
tick_sched_do_timer
tick_do_update_jiffies64	# update jiffies
```

**who legacy timer working after hres**

切换hres后，低精度timer raise timer softirq的动作在tick_sched_timer中触发，timer softirq在中断下半部被处理。

`hrtimer_interrupt`

hrtimer提供的tick device handler，切换到hres后，替换掉原来的tick_handle_periodic回调。

会从timerqueue中取出timer运行，运行后根据timer到期时间设置下次tick时间。