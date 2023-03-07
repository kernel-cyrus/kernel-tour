# stop

**Stop Class**

`/kernel/sched/stop_task.c`

stop调度类是优先级最高的调度类，允许以最高优先级运行一个stop task，这个task不能会被抢占，会一直运行到task结束或者主动让出CPU。

这个调度类的核心，是在rq上增加了一个stop的task struct指针，有stop task时，就以最高优先级执行。每个rq只有一个stop task。

（task_group可以有stop task吗？不能，因为没有添加接口。）

每个CPU rq的stop task可以通过 `sched_set_stop_task` 添加。

**Stop Machine**

`/kernel/stop_machine.c`

Stop Machine是基于stop task实现的，它在每个CPU对应一个单个stop task的基础上，实现了一个stop works的work链表，允许异步方式添加stop work。

Stop Machine的核心是实现了一个stop task，绑定在每个CPU上，这个task的loop中会遍历执行stop works。

```
cpu_stop_threads ("migration/<cpu>")
	create -> sched_set_stop_task -> add to rq->stop
	thread_fn -> cpu_stopper_thread (stop线程主函数)
```

percpu绑核的smp_hotplug_thread，名字是"migration/\<cpu\>"。

创建时绑定在每个CPU对应的rq->stop上。

`cpu_stop_thread`

stop task 主线程，会loop所有cpu_stoper->works并执行

`cpu_stopper`

Stop Machine的全局变量，主要包括了stop thread，和works链表。

**Stop Interface**

Stop Machine提供了以下接口可以添加Stop任务：

`stop_one_cpu`

为当前CPU添加stop work，同步等待执行结束。

`stop_one_cpu_nowait`

为当前CPU添加stop work，不等待结束直接返回。

`stop_two_cpu`

为当前CPU和另一个CPU添加stop work，同步等待执行结束。

`stop_machine`

为所有CPU添加stop work，同步等待执行结束。

**使用场景**

一般来说, 内核会在如下情况下使用到stop_machine：

- load balance
- module install/remove
- cpu hotplug
- memory hotplug
- hotpatch