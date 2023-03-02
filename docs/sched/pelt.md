# Per Entity Load Tracking (PELT)

**负载的定义**

一个task究竟对CPU会产生多大压力，或者说这个task究竟有多大？一个rq究竟有多大，多busy？内核需要一个指标，来衡量一个任务的大小（负载）以及rq的繁忙程度。

se(sched_entity)和rq(runqueue)中，都包含了一个 `struct sched_avg` 结构体，来记录这个负载信息。

负载(load)，不同于CPU占用率(utility)。一个task的CPU占用率，只是task在CPU上running时占用的时间。由于处在rq中尚未运行的task仍然可以认为对CPU产生压力，所以内核引入了负载，来综合反馈这些信息。所以负载包括了两部分信息：一段时间内，一个task的running时间，和ready的时间（也就是runnable状态的总时间）

**负载的计算原理**

负载如何计算？

内核将负载统计划分成粒度为1ms(1024us)的时间窗口

```
[<- 1024us ->|<- 1024us ->|<- 1024us ->| ...
     p0            p1           p2
    (now)       (~1ms ago)  (~2ms ago)
     u0            u1           u2
```

每个窗口统计task处在runnable状态的时间，记录为u，则每次统计出的负载为：

```
u = u0 + u1*y + y2*y^2 + u3*y^3 + ...
```

其中，y是一个常数：`y^32 = 0.5`

也就是说，每一时刻统计的task负载，等于当前窗口的负载 + 历史负载的衰减

**负载计算的实现**

由于调度并不能做到每个时间窗口精确地遍历并计算更新一遍所有task的负载（nohz），所以一个时刻task的负载信息计算，只在task的状态切换时完成（比如SLEEP->RUNNABLE，RUNNABLE(READY)=>RUNAABLE(RUNNING)）。

也就是说，每次计算负载，pelt只拿到了：上一次更新的时间，上一次的负载，以及本段运行时间task所处的运行状态（比如是sleep，还是runnable/running）。在更新是，会认为从上一次统计到本次更新，task一直处在这个状态，让后把本轮负载计算结果更新上去。

所以在这样的更新方式下，原有的计算方式变成了这样：

```
             d1          d2           d3
             ^           ^            ^
             |           |            |
           |<->|<----------------->|<--->|
   ... |---x---|------| ... |------|-----x (now)
           |                             |
    last_update_time                    now
                             p-1
   u' = (u + d1) y^p + 1024 \Sum y^n + d3 y^0
                             n=1
  
      = u y^p +					(Step 1)
  
                       p-1
        d1 y^p + 1024 \Sum y^n + d3 y^0		(Step 2)
                       n=1
```

其中：d1是上一次更新后到遇到第一个时间窗口结尾的时间，d2是连续完整的时间窗口，d3是最后一个不完整的时间窗口。step1计算的是之前负载的衰减结果，step2计算的是本轮负载统计更新上去的结果。这样，这个公式就只需要知道last_update_time，now，就可以根据本轮统计task所处的运行状态更新对应的负载信息。这就是pelt中实现的`accumulate_sum`所实现的功能。

`accumulate_sum`

```
(past) d1 [   d2   ] d3 (now)
           perioods

[period_contrib + d1 ] [ ... d2 ... ] [ d3 + remain ]
                |---------- delta ---------| delta'
|--------------------delta ----------------| delta''
                                      |----| delta''' => period_contrib
```

函数中对应变量如图，periods本轮统计经过的时间窗口个数。period_contrib是上轮结束的不完整窗口时间。delta经过了几轮计算和服用，变更过程如图，表示对应时间。

经过计算后，就得到了负载的sum值。

我们看到`sched_avg`下包括了三个sum：

load_sum:	task在ready + running状态的负载统计
util_sum:	task在running状态的负载统计
runnable_sum	task在ready状态的负载统计

（实际task只有runnable状态，这里的ready表示在rq的runnable，running表示在运行的runnable）

所以`accumulate_sum`的load, runnable, running传参，实际表示的是task在这段时间，是否处于runnable(ready+running)，ready，running的状态。从而分别统计对应的负载，记录到对应变量里。

sum在计算完成后，还需要计算真正的avg：`___update_load_avg`

avg就是sum/periods，只是其中load_avg比较特殊，会叠加task的weight（权重，从nice值映射过来），最终导致load_avg不大于se->load.weight。`___update_load_avg`的参数`load`需要传入的就是这个load.weight

这样我们就得到了一个task的几类描述负载的数据。

**负载的更新时机**

那么，负载在什么时候被更新？

从`accumulate_sum`的接口入参上，我们可以看到，更新负载的时机点，需要是task的状态的切换点，并且需要在这个统计点与上个统计点之间，task处于固定的状态。

- `enqueue_task_fair` / `dequeue_task_fair`
- `enqueue_entity` / `dequeue_entity`
- `put_prev_entity` / `set_next_entity`
- `entity_tick`
- `attach_entity_cfs_rq` / `detach_entity_cfs_rq`
- `__update_blocked_fair` / `propagate_entity-cfs_rq` / ...

**负载数据的用途**

sched_avg中的load数据，最终会用作cpufreq、负载均衡判断的依据。

**负载Tracing**

pelt中定义了负载更新时的trace hook，可以挂自己的处理函数：

```
DECLARE_TRACE(pelt_cfs_tp,
DECLARE_TRACE(pelt_rt_tp,
DECLARE_TRACE(pelt_dl_tp,
DECLARE_TRACE(pelt_thermal_tp,
DECLARE_TRACE(pelt_irq_tp,
DECLARE_TRACE(pelt_se_tp,
DECLARE_TRACE(sched_cpu_capacity_tp,
DECLARE_TRACE(sched_overutilized_tp,
DECLARE_TRACE(sched_util_est_cfs_tp,
DECLARE_TRACE(sched_util_est_se_tp,
DECLARE_TRACE(sched_update_nr_running_tp,
```

## Files

```
- /sched/pelt.c
```

## Reference

PELT

<https://lwn.net/Articles/531853>

<https://cloud.tencent.com/developer/article/1442276>