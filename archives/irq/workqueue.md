# workqueue

**MTWQ vs CMWQ（Concurrency Managed Workqueue）**

Kernel中原有的是MTWQ（multi thread workqueue）和STWQ（single thread workqueue）。对于MTWQ，每个workqueue要在每个CPU上创建一个worker线程，而STWQ是整个系统共用一个worker线程。这种情况下，MTWQ会创建出过多的worker线程，而STWQ，又会出现性能瓶颈，当worker线程被阻塞时，所有work都无法得到执行。

为了解决这些问题，提出了CMWQ的设计，这个设计的原则，核心就是动态管控每个cpu上worker线程的数量，尽量做到同一时间每个核上，都有一个且只有一个running的worker线程，尽量做到worker线程足够少，同时又不阻塞work执行。

**Worker Pool**

worker pool是worker的线程池，用来创建具有统一属性的worker线程。

系统会创建两类worker pool：

normal workerpool，每个CPU两个，一个正常优先级（pool-0）一个高优先级（pool-1），两个pool创建的kworker线程优先级不同，都会进行绑核，只能在该cpu运行。

unbound workerpool，每个Node一个（实际也就一个），创建出来的worker线程可以被调度到各个cpu。

用ps查看，kworker就是各个pool创建出来的worker线程，可能有几种格式：

```
    7 root     [kworker/0:0H-ev]
   52 root     [kworker/0:1H-ev]
   33 root     [kworker/0:1-eve]    
  109 root     [kworker/0:2-eve]
   19 root     [kworker/1:0-mm_]
   34 root     [kworker/1:1-mm_]
   20 root     [kworker/1:0H]
  214 root     [kworker/u8:0-ev]
  217 root     [kworker/u8:1-ev]
  219 root     [kworker/u8:2-ev]
```

kworker/CPU_ID:WORKER_ID：某个CPU上，normal pool 0创建的正常优先级worker线程
kworker/CPU_ID:WORKER_IDH：某个CPU上，normal pool 1创建的高优先级worker线程
kworker/uPOOL_ID:WORKER_ID：全局unbound pool创建的可被调度worker线程

通过taskset -p <pid>查看这些kworker线程，可以看到其被设置了对应的affinity和nice。

**Woker**

worker是由workerpool创建出来的一个内核线程(kthread)，他是一个空壳线程，用来处理来源不同的work。

workerpool在创建时会创建一个默认的worker，后续worker数量会由workerpool根据work数量和worker的运行情况来动态管理创建。

`worker_thread` 是worker thread的线程函数，被唤醒会检查worker pool中是否有work，有work则调用work的func处理掉该work，循环直到pool中没有work。

**Worker的动态管理**

> This allows using a minimal number of workers without losing execution bandwidth

worker pool中的worker数量是根据里面的work数量动态创建的。CMWQ的核心策略是保持一个worker处于running状态，如果suspend，就需要增加一个进入running，但是同一时间，running一个就够了，所以running一旦大于1，会让多余的running worker进入idle，如果idle过多，300s内没被使用的worker会被销毁。

> Keeping idle workers around doesn’t cost other than the memory space for kthreads, so cmwq holds onto idle ones for a while before killing them.

这里有个问题，如果只有一个running，如果这个work处理时间过长，其他work会阻塞过久。所以work所在的pool_workqueue又一个属性，WQ_CPU_INTENSIVE，设置后，这个work会独立占用一个worker，像进入suspend的情况，其他线程继续共用一个running。

**Work**

work是一个具体的函数参数组合，对应worker线程具体执行的function和参数

**Workqueue**

workqueue用来管理work的属性和work的分配。

每个workqueue中有一组pwq，pwq指向了percpu的normal pool和unbound pool，每个pool对应一个pwq，workqueue通过pwq来找到其work可以去的pool。pwq是work的推荐者，work会通过pwq进入最终该去的pool。

当一个work被插入到workqueue时，会通过workqueue是否是unbound来判断这个work应该去normal pool还是unbound pool，并找到对应pwq，然后再通过pwq把work插入到对应pool去。

在选核上，如果指定了work要去的cpu，则会去对应cpu的pool，如果没有指定，则会使用当前cpu的pool，如果是unbound，则进入调度。

系统默认创建了system_wq等七个workqueue，可以供work直接使用，这样就不需要自己创建独立workqueue。

**总结**

最终流程：

1. 掉用workqueue添加work的接口，work会被添加到对应的workqueue中（系统某个默认的，或者所指定的）
2. 进入workqueue后，会通过对应CPU的pwq，被安排到对应CPU的worker pool的work list上
3. 具体是哪个worker pool，由workqueue创建时的类型决定
4. 进入work list的work，会由worker pool的worker（内核线程）遍历，并调用work函数完成工作。
5. 同一时间一个worker pool只会有一个worker线程在处理work，除非这个work suspend掉了worker线程。
6. work suspend掉worker线程后，对于worklist中这个work queue的work，最多可以suspend+running几个，由work queue的@max_active决定

## Files

```
- /kernel/workqueue.c
- /include/linux/workqueue.h
```

## Variables

`cpu_worker_pools`

percpu的默认两个workerpool

`workqueues`

全局workqueue链表，在alloc_workqueue时被加入到链表

## Functions

`workqueue_init_early`

初始化所有workerpool：每个核两个默认pool

初始化所有默认workqueue：

```
extern struct workqueue_struct *system_wq;
extern struct workqueue_struct *system_highpri_wq;
extern struct workqueue_struct *system_long_wq;
extern struct workqueue_struct *system_unbound_wq;
extern struct workqueue_struct *system_freezable_wq;
extern struct workqueue_struct *system_power_efficient_wq;
extern struct workqueue_struct *system_freezable_power_efficient_wq;

```

并不是所有work都必要创建自己独立的workqueue，这些默认workqueue就是为那些没有自己workqueue的简短work提供服务的，可以直接被使用，实际上系统中大部分的场景，都是使用的这类默认workqueue

`workqueue_init`

为每个workerpool创建一个默认worker

`wq_sysfs_init`

workqueue会创建一个自己的subsystem(/sys/devices/virtual/workqueue)，并创建workqueue bus，alloc_workqueue时，支持传入WQ_SYSFS flag，在bus下创建workqueue virtual device，并暴露attrs节点，这些节点包括

由wq_sysfs_attrs定义的（default，注册bus时已设置）:
- per_cpu: whether the workqueue is per-cpu or unbound
- max_active： maximum number of in-flight work items

由wq_sysfs_unbound_attrs定义的（unbound）:
- pool_ids: the associated pool IDs for each node
- nice: nice value of the workers
- cpumask: bitmask of allowed CPUs for the workers
- numa: numa

`wq_watchdog_init`

当开启CONFIG_WQ_WATCHDOG时，会启动workqueue的wdt，设置一个wdt timer，workqueue超过30s会出发wdt dump，打印出"BUG: workqueue lockup - pool"，并通过 `show_all_workqueues` dump出所有workqueue、workerpool信息。workerpool会在调度过程中不断更新watchdog_ts，以避免触发wdt dump。

`show_all_workqueues`

dump出所有workqueue、workerpool信息

`alloc_workqueue`

申请一个workqueue

flags：
WQ_UNBOUND：不绑定到任何cpu上
WQ_HIGHPRI：是哟给你高优先级worker执行

max_active：
每个cpu上，最大可以同时执行的work数量

`destroy_workqueue`

`INIT_WORK`

`INIT_DELAYED_WORK`

`queue_work`

`queue_delayed_work`

`schedule_work`

`schedule_work_on`

`schedule_delayed_work_on`

`schedule_delayed_work`

## Reference

<https://www.kernel.org/doc/html/latest/core-api/workqueue.html?highlight=workqueue#c.drain_workqueue>

<http://kernel.meizu.com/linux-workqueue.html>

<https://embetronicx.com/tutorials/linux/device-drivers/workqueue-in-linux-kernel/>
