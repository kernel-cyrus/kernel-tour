# cfs

CFS sched class

思想
在CFS调度器中，每个线程，有自己的vruntime，这个vruntime的核心思想，是表示线程的运行时间，CFS的理想目标，是达到在任意时刻的所有task vruntime值相同，以做到“绝对公平”。CFS的runqueue是一棵vruntime排序的红黑树。（time-ordered rbtree），pick next task会从rbtree中选最左边的节点（vruntime最小）作为下一个调度对象。所以，CFS的调度行为大致是，每次从rq选择一个vruntime最小的执行，直到vruntime增长到不是最小，然后再次选择一个最小任务执行。
CFS只依赖与vruntime，使用时间单位，不依赖与jiffies或HZ，所以他感知不到任何时间片的信息。
CFS在/proc/sys/kernel/有sched节点，可以配置参数

调度周期
sched_latency_ns（6ms），保证了所有进程，在这个时间内，按weight划分各自执行时间，这个周期保证了所有线程在一个周期内都会被调度一次，因此保证了所有线程的实时性。存在的问题是如果线程数量过多，那么每个周期内线程能分配的时间就会很短，这会大幅增加上下文切换的次数，降低执行效率，所以，调度周期并不是一个固定值，在线程数大于8时，调度周期=nr_running * sched_min_granularity_ns（0.75ms）。见__sched_period()函数。sched_min_granularity_ns为线程运行的最小时间，这个值保证了线程至少要运行这个时间才会被切换。也就是说，线程数小于8时，按照weight分配时间，而线程数大雨8时，大家都是sched_min_granularity_ns运行时间。
sched_latency_ns / sched_min_granularity_ns可以在/proc/sys/kernel/设置
另外还有一个参数：sched_wakeup_granularity_ns
这个参数限定了一个唤醒进程要抢占当前进程之前必须满足的条件：只有当该唤醒进程的vruntime比当前进程的vruntime小、并且两者差距(vdiff)大于sched_wakeup_granularity_ns的情况下，才可以抢占，否则不可以。这个参数越大，发生唤醒抢占就越不容易。

vruntime / weight计算
vruntime = (wall_time * NICE0_TO_weight) / weight（权重越大，vruntime越小，越会被执行）
以下是40个nice值对应的weight，weight = 1024 / (1.25 ^ nice)，weight每档相差1.25倍
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
当前进程CPU时间的分配占比 = weight / 全部进程weight求和，也就是说，时间占比由weight决定，而weight有nice决定，nice每改变1，cpu时间改变10%。

vruntime的一些问题
1. 新入线程的vruntime会按照rq中的min vruntime来计算出。
2. 睡眠唤醒后，vruntime会按照min vruntime来进行补偿

sched features
sched_features是控制调度器特性的开关，每个bit表示调度器的一个特性。在sched_features.h文件中记录了全部的特性。这些特性改变着vruntime的补偿行为，几唤醒线程优先执行的一些特性。
这些配置可以在sysfs节点修改：/sys/kernel/debug/sched_features

遗留问题：给进程分配的执行时间是如何控制的？需要搞清楚schedule的tick机制。

参考
https://www.kernel.org/doc/Documentation/scheduler/sched-design-CFS.txt
https://blog.csdn.net/yangcs2009/article/details/38796429
https://www.cnblogs.com/mfrbuaa/p/4641240.html
http://linuxperf.com/?p=42


newidle_balance!