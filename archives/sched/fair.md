# cfs

## 基本原理

CFS，Completely Fair Schedule，如其名字，希望建立一个对所有线程确定公平的虚拟并行环境。

CFS中的线程，用vruntime来记录其执行时间。CFS的理想目标，是达到在任意时刻的所有task vruntime值相同，以做到“绝对公平”。

cfs_rq是一个红黑树（`rb_root_cached`: `cfs_rq->tasks_timeline`），pick_next_task总是会选取最左侧节点（vruntime最小）作为下一个调度对象。

所以，CFS的调度行为大致是，每次从rq选择一个vruntime最小的执行，直到vruntime增长到不是最小，然后再次选择一个最小任务执行。

CFS的调度，只依赖于vruntime，使用时间单位，他不依赖jiffies或HZ，所以他感知不到任何时间片的信息。

Reference: <https://www.kernel.org/doc/Documentation/translations/zh_CN/scheduler/sched-design-CFS.rst>

## vruntime / weight 计算

`vruntime = (wall_time * NICE_TO_weight) / weight`

权重越大，vruntime就增长越慢，墙上执行时间就越长。

以下是40个nice值对应的weight，weight = 1024 / (1.25 ^ nice)，weight每档相差1.25倍

```
const int sched_prio_to_weight[40] = {
 /* -20 */     88761,     71755,     56483,     46273,     36291,
 /* -15 */     29154,     23254,     18705,     14949,     11916,
 /* -10 */      9548,      7620,      6100,      4904,      3906,
 /*  -5 */      3121,      2501,      1991,      1586,      1277,
 /*   0 */      1024,       820,       655,       526,       423,
 /*   5 */       335,       272,       215,       172,       137,
 /*  10 */       110,        87,        70,        56,        45,
 /*  15 */        36,        29,        23,        18,        15,
};
```

当前进程CPU时间的分配占比 = weight / 全部进程weight求和，也就是说，时间占比由weight决定，而weight有nice决定，nice每改变1，cpu时间改变10%。

**vruntime的更新**

vruntime是一个64bit的数，task的vruntime在其创建后只要运行过就会不断增长，这就会导致两个执行历史不一样的任务，在同时进入rq时，其vruntime并不一致，导致不公平。

所以，rq维护了一个队列min vruntime的值，在每一个task入队时，会将task的vruntime设置为队列的min vruntime，从而保证任务在进入rq时，任务有一直的绝对vruntime。

cfs在调度时，只考虑rq中任务的vruntime的相对值，所以在vruntime增长到一定程度后，会将所有rq任务的vruntime减掉min vruntime，将所有task在rq中的基准vruntime清零。

task的vruntime可以在/sys/kernel/debug/sched/debug中查看。

## 调度周期、调度粒度

调度周期用于保证在一个周期内，所有线程都能至少得到一次调度，保障线程的最大调度延迟。

调度粒度用于调度周期的计算，当rq中的线程数少于8时，调度周期使用固定设置值（/proc/sys/kernel/sched_latency_ns），当rq中线程数大于8时，调度周期为粒度乘以rq中的线程数（粒度：/proc/sys/kernel/sched_min_granularity_ns）

这么计算的原因是避免线程多时，固定的调度周期被切分到太细，导致线程抢占过于频繁发生，影响整体吞吐量。

## 组调度（group scheduling）

**背景**

从前CFS没有task group，在多用户共享一台服务器的情况下，CFS针对task的绝对公平，会导致用户间的不公平。

比如Alice运行了1个进程，而Bob运行了99个进程，Alice实际只能得到1%的运行时间。

为了解决这个问题，CFS提出后不久，就有人提出了group tasking的解决方案。

**实现**

这个方案增加了sched_entity作为调度对象，用来表示“一个可以被调度的东西”，这个东西既可以是一个task_struct，也可以是一个task_group，而task_group是一个task_struct的组合。

每个task_group有自己独立的rq，这样如图，多级rq组成了分层结构：

```
CPU 0: rq
	|-- dl_rq
	|-- rt_rq
	|-- cfs_rq
		|--- sched_entity -- sched_entity
                     (task_struct)    (task_group)
                                          |-- cfs_rq
                                                 |-- sched_entity -- sched_entity
```

在选取下一个任务时，会自顶向上安层找到最终底层rq中vruntime最小的节点。

有了这样一个结构，管理员就可以为Alice和Bob分别创建两个task_group，然后往各自的group下创建task，CFS会保证主rq的绝对公平，这样Alice和Bob都可以得到50%的运行时间。

组调度的实现如图所示：

```
                CPU rq
                   | enqueue
task_group  - sched_entity        rq
                   | parent       | enqueue
task_group  - sched_entity -------+   rq
                   | parent           | enqueue
task_struct - sched_entity -----------+ (start to run)
```

task在创建时，就关联到了对应的task_group，通过sched_entity.parent来记录这个所属关系。

task未运行时，这个关系记录在sched_entity.parent中。

task运行时，进行enqueue操作，在使能group scheduling功能后，这个enqueue动作会自底向上分层enqueue。

每一层enqueue，将自己的sched entity入队到其所绑定的rq中（父entity的rq），最上层的sched entity入队到CPU主rq，完成整个enqueue操作。

这里需要注意一点，sched_entity在进程创建时创建，在进程运行时enqueue，不运行时不在rq中。

组调度在实现时分为三层：

1、sched/core中实现了所有task group的操作接口（create group，add task等），并基于cgroup实现了一个cgroup subsys（`cpu_cgrp_subsys`）

2、cgroup使用sched/core注册进来的subsys，为用户空间创建出操作接口

3、sched/fair，sched/rt中，实现了各自调度类对task group这类sched entity的支持。

CFS、RT都支持task group调度（FAIR_GROUP_SCHED，RT_GROUP_SCHED），STOP、DL、IDLE不支持。

**使用**

```
# mount -t tmpfs cgroup_root /sys/fs/cgroup
# mkdir /sys/fs/cgroup/cpu
# mount -t cgroup -ocpu none /sys/fs/cgroup/cpu
# cd /sys/fs/cgroup/cpu

# mkdir multimedia	# 创建 "multimedia" 任务组
# mkdir browser		# 创建 "browser" 任务组

# #配置multimedia组，令其获得browser组两倍CPU带宽

# echo 2048 > multimedia/cpu.shares
# echo 1024 > browser/cpu.shares

# firefox &	# 启动firefox并把它移到 "browser" 组
# echo <firefox_pid> > browser/tasks

# #启动gmplayer（或者你最喜欢的电影播放器）
# echo <movie_player_pid> > multimedia/tasks
```

Reference: <https://lwn.net/Articles/240474/>

## CFS Bandwidth Control

`COFNIG_CFS_BANDWIDTH`

Reference: <https://docs.kernel.org/scheduler/sched-bwc.html>
