# rt

## 数据结构

RT调度类是完全按照优先级进行调度的实时调度类。

进程优先级中，1\~99是RT的优先级。

RT的底层数据结构是rt runqueue，是一个优先级数组链表，结构如下：

```
rq->rt_rq->active
	queue[0]: rt_se->run_list <-> rt_se->run_list <-> rt_se->run_list
	queue[1]: ...
	queue[2]: ...
	...
	queue[99]: ...
```

rt_rq中用一个bitmap来表示非空的rq list。

rt_sched_entity->run_list是同一优先级的rt task链表。

## 调度策略

进程在加入RT调度器时，可以选择使用的策略：

`sched_setscheduler(pid, policy, param)`

- SCHED_FIFO：先进先调度
- SCHED_RR：时间片轮转

如果进程使用FIFO，则会完全按照优先级顺序和入队顺序执行。

如果进程使用RR，则在tick到来时，如果同优先级有其他任务，则会轮转执行。（`task_tick_rt`）

## 负载均衡

RT的负载均衡也比较简单，主要为了避免一核有难，其他核围观的情况发生。

RT中每个rq用一个push链表来管理可以迁核的rt task。

负载均衡包括两类情况：

1、balance时，从其他核pull进程过来：

- `__schedule/__balance_callbacks`时从其他核尝试pull进程过来。
- other balance callback（`balance_callback`, `__balance_callback`, ``class->balance->balance_rt``）

2、当前rq负载过重时，向低负载核迁移任务：

- 每次pick next task结束，会push balance callback，在post schedule阶段，会执行`push_rt_tasks`，如果当前rq过载，则会将当前rq的pushble列表的task往其他低负载的rq上迁移。


## 使用和调试

向RT放置进程：

```
chrt -r -p <1~99> <pid>
```

## 文件

```
/kernel/sched/rt.c
```