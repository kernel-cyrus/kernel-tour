# dl

文件：`/sched/deadline.c`

deadline是为了满足确定时长任务的实时性而创建的调度类。

deadline任务在创建时，需要指定三个属性：

- runtime（任务运行时间）
- period（一个周期时长）
- deadline（周期内的最晚结束时间）

这三个时间如下图所示：

```
 execution         execution   scheduling deadline
   start              end     /
|---------------------------------|---------------------------------|
|    |                 |     |    |          |                 |    |
|    |<--- runtime --->|     |    |          |<--- runtime --->|    |
|                            |    |                            |    |
|<-------- deadline -------->|    |<-------- deadline -------->|    |
|                                 |                                 |
|<----------- period ------------>|<----------- period ------------>|
```

任务创建后，每个period周期会被调度一次，并在deadline结束前，找一个合适的位置，安排runtime的运行时长。

在一个CPU上，假设有三个Task：

- T1: runtime=1, deadline=4, period=4
- T2: runtime=2, deadline=6, period=6
- T3: runtime=3, deadline=8, peroid=8

调度会如下图所示（1=4个格子）：

```
T1	|====           |        ====   |    ====       |    ====       | ...

T2      |    ========           |    ========           |        ======== ...

T3      |            ============       |        ============           | ...
```

所以deadline的任务安排，实际是一个装箱问题。在新任务创建时，会判断下是否能排得进去，如果排不进去，就会创建失败。

**EDF (Early Deadline First)**

当一个deadline任务运行结束，选择哪个任务来运行？

deadline调度器使用EDF算法，会选择deadline最近的一个任务来执行。

这也导致一个问题，就是在多核上，deadline较早的小task可能提前占掉所有CPU，新来的大Task可能无法arrange到运行时间。这个问题就是dhall's effect（有资源不代表能执行），如图所示，尽管所有CPU都很空闲，但是T5没有办法继续创建。

```
CPU 0:	|<t1>			|
CPU 1:	|<t2>			|
CPU 2:	|<t3>			|
CPU 3:	|<t4><-------t5---------|->
```

实现上，deadline的pick_next_task使用红黑树来维护一个按deadline排序的task优先级队列。

**CBS （Constant Bendwidth Server）**

一个task的`bandwidth = runtime / period`，Constant Bendwidth，就是维持每个deadline task的bandwidth固定。

为此CBS需要管控task的准入（Runqueue Bandwidth），及task运行完自己runtime后的切出（Running Bandwidth）。

这需要两个变量的参与：

- scheduling deadline（deadline的绝对时间）
- remaining runtime（runtime剩余时间）

新增的任务，根据检查，如果检查满足，则允许创建，不满足则不能创建：

```
         remaining runtime                  runtime
----------------------------------    >    ---------
scheduling deadline - current time           period
```

任务初创建时：

```
scheduling deadline = current time + deadline
remaining runtime = runtime
```

任务运行一段时间(t)后：

```
remaining runtime = runtime - t
```

当runtime执行完，remaining runtime为0时，任务被"throttled"强行终止，直到下一个period到来（replenishment time）。

当时间到达"throttled"任务的replenishment time时，变量被重置：

```
scheduling deadline = scheduling deadline + period
remaining runtime = remaining runtime + runtime
```

**Reclaiming**

一个deadline task，可能因为被阻塞(blocked)，或者主动让出的原因，在没有运行完自己的runtime时间时切出。这时，如果其可以在0-lag时间内切回来完成自己的运行，则其他task不会使用它的runtime，如果它错过了自己的0-lag时间，则它在这个period没有机会再运行，它让出来的runtime会免费赠送给其他task使用。（被切进来的task reclaim掉）

deadline的进程，内部会三个状态：

- (1) Active Contending（runnable or running）
- (2) Active Non-Contending（Blocked, 但是时间还够，还有机会正常运行完）
- (3) Inactive（Blocked，没机会运行完了）

task1因为block的原因，从(1)进入(2)，会起一个timer，在0-lag时间到期时检查

同时，task2开始运行

task2在运行到0-lag timer到期时，如果task1运行了，则timer被取消，如果没运行，则task1进入(3)

在task1进入(3)后，task1处于(2)的时间被task2回收，相当于白让task2运行了这么久，然后task2继续按照自己的runtime运行。

一个task也可以在运行过程中通过sched_yield()让出自己的bandwidth，让出来的运行时间会赠送给下一个task使用。

**Create Deadline Task**

Run process in deadline:

```
chrt -d -T <runtime> -D <deadline> -P <period> 0 <command> <command options>
```

Change scheduling policy for an existing process:

```
chrt -d -T <runtime> -D <deadline> -P <period> -p 0 <PID>
```

Example:

```
chrt -d --sched-runtime 1000000 --sched-deadline 5000000 --sched-period 5000000 0 ./test-app &
```

Reference:

<https://access.redhat.com/solutions/3742421>

Other tools:

[schedtool-dl](https://github.com/scheduler-tools/schedtool-dl>)

to run applications in deadline scheduler

## Reference

Kernel：

<https://www.kernel.org/doc/html/next/scheduler/sched-deadline.html>

LWN:

<https://lwn.net/Articles/743740/>

<https://lwn.net/Articles/743946/>

LWN（翻译）：

<http://www.wowotech.net/process_management/deadline-scheduler-1.html>

<http://www.wowotech.net/process_management/dl-scheduler-2.html>

Other：

<https://blog.csdn.net/chensong_2000/article/details/125201662>

Deadline for Android?

<https://lwn.net/Articles/706374/>