# dl

DL sched class
deadline为了满足确定时常任务实时行性的调度类。
deadline任务有三个属性：runtime（运行时常），deadline（超时时间），period（周期时间）
设置好三个属性后，DL调度器会帮我们在超时前安排任务执行完毕。（EDF算法 early deadline first）
EDF
EDF算法最早触发deadline的先执行，这也是cause Dhall’s effect的原因。pick next task使用一个EDF算法排序的红黑树来维护优先级，在enque和dequeue时，这个红黑树总是一个按deadline排序过以后的rq。
CBS （running bandwidth & rq bandwidth）
如果新增加的任务不能被安排（运行检查 acceptance test），则会拒绝任务进入DL调度器。
如果任务超过预定的运行时间，则其运行会被CBS（constant bandwidth server）机制throttled。bandwidth包括rq的和running的两部分，调度余量叫做bandwidth reclaiming，GRUB算法会最大回收调度余量。如果一个task没有使用完他的running time，可以用sched_yield()退回。
Contending
如果两个task发生竞争（contending）则一个task会被置为(active non condending)状态，并起一个timer，等待前一线程到期（0-lag时间）。如果提前唤醒，则取消timer。这个timer用来保证task在指定时间能得到调度。
总结：将thread放入DL rq只需在线程中调用sched_setattr设置policy，runtime，period，deadline即可，支持用户线程。每次进出rq，都会根据EDF算法对rq进行排序，保证dl执行顺序正常。