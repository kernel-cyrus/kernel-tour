# schedutil

schedutil governor负责从sched framework中获取util信息，根据util触发CPU调频动作。

util信息由pelt或者walt写入，schedutil会向sched注册负载变化的回调函数，当负载发生变化时，schedutil回调执行。

整个schedutil软件框架和调频流程如下图所示：

```
		[  dl  ]  [ fair ]  [  rt  ]
		    |         |         | (cpufreq_update_util)
		    +---------+---------+
		              | sched_utile_handler (sugov_update_single_freq)
		        [ sched_util ]
		fast_switch |    | irq_work
    cpufreq_driver_fast_switch   IRQ: kthread_queue_work
                            |    |
                   call driver   deadline thread: sugov_work
                                 |
                                 __cpufreq_driver_target
```

主要数据结构如下：

```
sugov_cpu->update_util		# 负载变化回调函数
sugov_cpu->util			# 当前util
sugov_policy->irq_work		# 调频中断上半部
sugov_policy->work		# 调频处理线程（下半部）
sugov_policy->worker		# kthread worker（用于一个线程处理多个work）
sugov_policy->thread		# deadline中的调频线程，当有work queue入时执行
sugov_policy->next_freq		# 目标频点
```

主要流程如下：

```
sugov_init
	sugov_kthread_create
		创建thread，worker，初始化work，irq_work
		其中，irq_work为调频上半部，work为调频线程处理函数，work会queue入thread的worker中执行。
```

```
sugov_start
	init sg_policy
	init sg_cpu
	cpufreq_add_update_util_hook（向调度器注册util更新响应函数，三选一）
	  |--sugov_update_shared
	  |--sugov_update_single_perf
	  +--sugov_udpate_single_freq
```

```
cpufreq_update_util	# rt、dl、fair的util发生变化时触发
	sugov_update_single_freq
		sugov_get_util	# 获取当前rq的util值
			effective_cpu_util
		get_next_freq	# 通过util值计算目标频点
		sugov_update_next_freq	# 将调频信息更新到policy中
		if fast switch
			（线程上下文）cpufreq_driver_fast_switch -> 调用cpufreq driver设置频点
		if not fast switch
			irq_work_queue(sugov_irq_work)，触发IRQ_WORK IPI中断，执行sugov_irq_work
			（中断上半部）在IPI中断中执行sugov_irq_work，向worker中添加work
			（中断下半部）在deadline thread的worker中，执行work-->sugov_work
			            sugov_work调用__cpufreq_driver_target触发调频
```

文件：`/kernel/sched/cpufreq_schedutil.c`

参考：<https://docs.kernel.org/scheduler/schedutil.html>