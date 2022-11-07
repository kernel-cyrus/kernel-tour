# hrtime


hrtimer定义了8组timer base，可以使用不同时间类型做计时。

hrtimer也实现了debug object，可以对用户申请释放过程做预防监控。

## Files

```
- /include/linux/hrtimer.h		# interface
- /kernel/time/hrtimer.c		# implementation
- /include/linux/timerqueue.h		# timer rb-tree
```

## Structures

`struct hrtimer`

hrtimer基本结构体，主要定义回调函数、到期ktime、使用的clock base、ktime是否为相对时间、硬timer还是软timer

`struct hrtimer_cpu_base`



`struct hrtimer_clock_base`


struct timerqueue_node {
	struct rb_node node;
	ktime_t expires;
};

struct timerqueue_head {
	struct rb_root_cached rb_root;
};


## Variables

`hrtimer_bases`


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

