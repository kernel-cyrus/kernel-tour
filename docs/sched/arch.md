# sched architecture

From the very begining of scheduling

## Data Structures

```
cpu0 - rq - curr   - *task_struct
          - stop   - *task_struct
          - idle   - *task_struct
          - dl_rq  - [sched_entity | sched_entity | ... ]
          - rt_rq  - [sched_entity | sched_entity | ... ]
          - cfs_rq - [sched_entity | sched_entity | ... ]
                           |              |
                       task_struct        +-- rq [sched_entity...]
                                          |
                                          |
cpu1 - rq ...             sched_entity <--+-- rq [sched_entity...]
cpu2 - rq ...             sched_entity <--+-- rq [sched_entity...]
cpu3 - rq ...             sched_entity <--+-- rq [sched_entity...]
                                          |
                                     task_group
```

1、每个cpu有一个rq

2、每个rq包括了stop task、idle task和cfs、rt、dl三个rq（对应五个调度类）

3、cfs_rq、dl_rq的结构是一个rb tree，rt_rq的结构是一个priority list（分级列表）

4、cfs_rq、rt_rq、dl_rq中，分别使用sched_entity、sched_rt_entity、sched_dl_entity来组织调度对象

5、sched_entity可以对应(内嵌在)一个task_struct，也可以对应(内嵌在)一个task_group

6、task_group在每个cpu上都有一个sched_entity，在每个cpu上也有一个独立rq，这个rq的二叉树相当于接到了rq的二叉树上。

调度时，会按照stop - dl - rt - cfs - idle的顺序，遍历五个调度类的数据结构，找一个task来运行。

**Task**

`struct task_struct`

```
pid			# PID
comm			# name
---------------------
thread_info
	ttbr0_el1
thread_struct
	cpu context
---------------------
sched_entity
sched_rt_entity
sched_dl_entity
---------------------
sched_task_group	-> group belongs to
---------------------
list			# all process list ->
parent			# parent process
children		# child process
siblings		# brother process
```

一个进程有一个task_struct，保存着进程状态和所有相关信息。

其中，进程的地址空间分为用户空间(ttbr0_el1)和内核空间(ttbr1_el1)

有用户空间的线程叫用户线程，没有用户空间的线程叫内核线程(ttbr0_el1)

所有进程共享内核空间(ttbr1_el1)。

entity内嵌在task_struct中，用来组成对应rq的数据结构。

`struct task_group`

```
---------------------
sched_entity		# schedulable entities of this group on each CPU
sched_rt_entity         # 每个CPU一个
---------------------
cfs_rq			# runqueue "owned" by this group on each CPU
rt_rq
---------------------
struct rcu_head rcu;
struct list_head list;	# tasks in this group?
---------------------
struct task_group *parent;
struct list_head siblings;
struct list_head children;
```

task_group中包含是一组task，在每个cpu上，都有一个sched_entity，放在这个cpu的rq中。另外，他自己还有一个rq，这个rq相当于插入到了父rq的sched_entity节点中。

task_groups：list of all task groups

root_task_group: default task group

**Entity**

`struct sched_entity`

cfs调度类使用的调度对象，一个entity可以对应一个task_struct，也可以对应一个task_group

`struct sched_dl_entity`

dl调度类使用的调度对象

`struct sched_rt_entity`

dl调度类使用的调度对象

**Runqueue**

`struct rq`

每个cpu一个rq，存放着这个cpu所有task，以及待运行的task。

rq是调度器使用的顶层数据结构。

`struct cfs_rq`

cfs调度了的rq，其中rb_root_cached是整个红黑树的root节点，rb_left_most指向了最左侧节点。

`struct dl_rq`

dl调度了的rq，其中rb_root_cached是整个红黑树的root节点，rb_left_most指向了最左侧节点。

`struct rt_rq`

rt调度了的rq，其中rt_prio_array保存了整个task优先级数组

**Bandwidth**

`struct cfs_bandwidth`

`struct dl_bandwidth`

`struct rt_bandwidth`

**Thread**

`struct thread_info`

`struct thread_struct`

`struct cpu_context`

**Statistics**

`struct sched_info`

`struct sched_statistics`

`struct sched_avg`

**Scheduler**

`sched_class`

sched_class是一个基类，其他5个调度类都是这个类的子类

- enqueue_task: 将task放入rq

- dequeue_task: 将task移出rq

- pick_next_task: 选择一个最高优先级的task

- put_prev_task: 每当task被移出CPU时（抢占、suspend等）

- task_tick: tick回掉（给schedule class接收tick事件，主要用于设置need_resched标记）

## The Scheduler Init

```
sched_init

	init task_groups

	init root_task_group

	init percpu rq

	init rt、dl、cfs、idle sched class

	init idle

sched_init_smp


```

## The First Process Creation

`task_struct init_task`

Primary CPU启动时的第一个默认内核线程，也就是PID(0):swapper/0，在init_task.c中静态定义。

在主核所有初始化工作完成后，作为idle线程进入idle loop。

Secondary CPU在启动时，会copy init_task，为各个CPU创建出swapper/\<cpu\>线程，最终进入idle loop

```
#0  init_idle ()		at kernel/sched/core.c:8907
#1  fork_idle (cpu=cpu@entry=1) at kernel/fork.c:2573
#2  idle_init (cpu=1) 		at kernel/smpboot.c:55
#3  idle_threads_init () 	at kernel/smpboot.c:74
#4  smp_init () 		at kernel/smp.c:1106
#5  kernel_init_freeable () 	at init/main.c:1602
#6  kernel_init () 		at init/main.c:1499
#7  ret_from_fork () 		at arch/arm64/kernel/entry.S:867

```

## Process Lifecycle

用户进程通过fork、vfork、clone系统调用创建：

```
#0  kernel_clone () 		at kernel/fork.c:2615
#1  __do_sys_clone () 		at kernel/fork.c:2789
#2  __se_sys_clone () 		at kernel/fork.c:2757
#3  __arm64_sys_clone () 	at kernel/fork.c:2757
#4  __invoke_syscall () 	at arch/arm64/kernel/syscall.c:38
#5  invoke_syscall ()		at arch/arm64/kernel/syscall.c:52
#6  el0_svc_common ()		at arch/arm64/kernel/syscall.c:142
#7  do_el0_svc ()		at arch/arm64/kernel/syscall.c:206
#8  el0_svc ()			at arch/arm64/kernel/entry-common.c:624
#9  el0t_64_sync_handler ()	at arch/arm64/kernel/entry-common.c:642
#10 el0t_64_sync () 		at arch/arm64/kernel/entry.S:581
```

内核线程通过kernel_thread接口创建：

```
#0  kernel_clone () 		at kernel/fork.c:2606
#1  kernel_thread () 		at kernel/fork.c:2706
#2  rest_init () 		at init/main.c:700
#3  arch_call_rest_init () 	at init/main.c:888
#4  start_kernel () 		at init/main.c:1146
#5  __primary_switched ()
```

底层创建均使用`kernel_clone`接口。

创建新的task_struct后，会通过`wake_up_new_task`，将新task添加到rq中，唤醒执行。

进程切换状态机如下：

```
READY:					TASK_RUNNING (READY) ---------------------+
                                                 |                                |
                        +------------------------+--------------------+           |
                        |                        |                    |           |
BLOCK:		TASK_INTERRUPTIBLE	TASK_UNINTERRUPTIBLE	TASK_KILLABLE     |
                        |                        |                    |           |
                        +------------------------+--------------------+           |
                                                 |                                |
RUNNING:				TASK_RUNNING (RUNNING) -------------------+
                                                 |
                                       +---------+-----------+
                                       |                     |
EXIT:				  TASK_ZOMBIE ---------> TASK_DEAD
```

其中，只要task在rq中，就是TASK_RUNNING的状态，这里包括了READY(PENDING)和RUNNING两种情况。

TASK_RUNNING (READY) -> TASK_RUNNING (RUNNING) : rq的任务被调度到（任务抢占 / 时间片轮转）

TASK_RUNNING (RUNNING) -> TASK_RUNNING (READY) : rq的任务被调出（主动出让CPU / 任务抢占 / 时间片轮转）

TASK_RUNNING <-> TASK_INTERRUPTIBLE : 等待/等到信号主动睡眠/唤醒（timer sleep / wait for signal）

TASK_RUNNING <-> TASK_UNINTERRUPTIBLE : 等待/等到IO（io）

TASK_RUNNING -> TASK_ZOMBIE : 父进程退出，等待子进程结束的Signal

TASK_RUNNING -> TASK_DEAD : 进程完全退出

TASK_ZOMBIE -> TASK_DEAD : 进程完全退出

\* 进程状态详细信息查阅<../process/state>

## Priority and Schedule Class

task_struct->prio取值范围是0～139，值越小，优先级越高。

RT优先级为1～99，CFS优先级为100～139

其中CFS的默认值为120，nice值取值范围为-20～19，CFS的最终动态优先级为priority+nice，工具中显示的，就是这个最终的优先级。

Priority在不同tools的显示不同，原因未知：

- 在proc/pid/sched中，对应0～139，默认120（task_struct原始值）静态priority

- 在top中，对应-100～39，默认20（这样比较好区分RT和CFS）动态priority（叠加了nice后的值）

- 在ps -l中，对应-40～99，默认80（可能因为兼容unix现实）动态priority（叠加了nice后的值）

## Tick Wheels

tick device负责周期性tick的触发，hrtimer+nohz模式下，允许在没有任务时不触发任何tick。

（这部分详见time system相关内容）

系统启动时会经过legacy timer -> hrtimer的切换，

legacy timer阶段 `tick_periodic`：

```
#0  tick_periodic () 				at kernel/time/tick-common.c:87
#1  tick_handle_periodic (dev=) 		at kernel/time/tick-common.c:112
#2  timer_handler (evt=) 			at drivers/clocksource/arm_arch_timer.c:653
#3  arch_timer_handler_virt (irq=, dev_id=)	at drivers/clocksource/arm_arch_timer.c:664
#4  handle_percpu_devid_irq () 			at kernel/irq/chip.c:928
#5  generic_handle_irq_desc (desc=) 		at ./include/linux/irqdesc.h:158
#6  handle_irq_desc (desc=) 			at kernel/irq/irqdesc.c:648
#7  generic_handle_domain_irq (domain=, hwirq=) at kernel/irq/irqdesc.c:704
#8  gic_handle_irq () 				at drivers/irqchip/irq-gic.c:372
#9  call_on_irq_stack () 			at arch/arm64/kernel/entry.S:896
```

hrtimer阶段 `tick_sched_handle`：

```
#0  tick_sched_handle () 			at kernel/time/tick-sched.c:231
#1  tick_sched_timer () 			at kernel/time/tick-sched.c:1480
#2  __run_hrtimer ()				at kernel/time/hrtimer.c:1685
#3  __hrtimer_run_queues ()			at kernel/time/hrtimer.c:1749
#4  hrtimer_interrupt (dev=) 			at kernel/time/hrtimer.c:1811
#5  timer_handler (evt=) 			at drivers/clocksource/arm_arch_timer.c:653
#6  arch_timer_handler_virt (irq=, dev_id=) 	at drivers/clocksource/arm_arch_timer.c:664
#7  handle_percpu_devid_irq (desc=) 		at kernel/irq/chip.c:928
#8  generic_handle_irq_desc (desc=) 		at ./include/linux/irqdesc.h:158
#9  handle_irq_desc (desc=) 			at kernel/irq/irqdesc.c:648
#10 generic_handle_domain_irq (domain=, hwirq=) at kernel/irq/irqdesc.c:704
#11 gic_handle_irq () 				at drivers/irqchip/irq-gic.c:372
#12 call_on_irq_stack () 			at arch/arm64/kernel/entry.S:896
```

最终调用`scheduler_tick` - `curr->sched_class->task_tick(rq, curr, 0)`检查是否有抢占（更高优先级或者轮转抢占）

如果需要换出，则调用`resched_curr`将当前进程换出。

```
#0  resched_curr (rq=) 				at kernel/sched/core.c:1033
#1  check_preempt_tick (curr=, cfs_rq=) 	at kernel/sched/fair.c:4495
#2  entity_tick (curr=, cfs_rq=) 		at kernel/sched/fair.c:4677
#3  task_tick_fair (rq=, curr=) 		at kernel/sched/fair.c:11182
#4  scheduler_tick () 				at kernel/sched/core.c:5417
#5  update_process_times () 			at kernel/time/timer.c:1844
#6  tick_sched_handle () 			at kernel/time/tick-sched.c:243
tick...
```

## The Switch

可以通过调用 `__schedule` 来触发调度，重新选择一个适合的task来执行。

调用 `__schedule` 的位置非常多，你也可以在自己的代码中通过手动调用 `schedule()` 来让出时间片。

大致上，在以下位置会触发调度：

1、tick到期时

`[function: update_process_times] scheduler_tick();`

2、进程主动Sleep

`[function: schedule_timeout] schedule();`

3、中断处理结束

`[function: __el1_irq] arm64_preempt_schedule_irq(); schedule();`

4、返回用户空间

`[function: do_notify_resume] schedule();`

5、其他还有大量的主动调用 `schedule`, `preempt_enable` 让出CPU的位置。

schedule的点，也就是下面的所有抢占行为的执行点。

## The Preeption

抢占，就是在自己线程运行时，被其他线程抢占进来运行。

**用户线程抢占**

`task_struct` -> `thread_info` -> `flags` 中有一个TIF_NEED_RESCHED标记，用于标记当前线程是否需要被重新调度。

在以下位置，线程会调用 `resched_curr` 将自己标记为需要重新调度。

- tick时间片到期 (`resched_curr`)

- sleep, wait for signal (`resched_curr`)

- send signal (`wake_up_process`, `try_to_wake_up`)

- fork (`wake_up_new_task`)

- 调整当前线程nice值 (`set_user_nice`)

用户线程被标记为RESCHED后，并不会立即触发schedule，而是需要等到一个抢占点。

用户线程只会在运行到以下几个位置时触发schedule，执行抢占：

- 异常处理后返回到用户态

- 中断处理后返回到用户态

- 系统调用后返回到用户态

比如，一个进程创建了一个优先级更高的进程，那么在sys_fork中，会被标记RESCHED：

```
#0  resched_curr () 		at kernel/sched/core.c:1033
#1  check_preempt_curr () 	at kernel/sched/core.c:2191
#2  wake_up_new_task ()		at kernel/sched/core.c:4647
#3  kernel_clone () 		at kernel/fork.c:2679
#4  __do_sys_clone ()		at kernel/fork.c:2789
#5  __se_sys_clone ()		at kernel/fork.c:2757
#6  __arm64_sys_clone () 	at kernel/fork.c:2757
#7  __invoke_syscall ()		at arch/arm64/kernel/syscall.c:38
#8  invoke_syscall () 		at arch/arm64/kernel/syscall.c:52
#9  el0_svc_common ()		at arch/arm64/kernel/syscall.c:142
#10 do_el0_svc ()		at arch/arm64/kernel/syscall.c:206
#11 el0_svc () 			at arch/arm64/kernel/entry-common.c:624
#12 el0t_64_sync_handler ()	at arch/arm64/kernel/entry-common.c:642
#13 el0t_64_sync () 		at arch/arm64/kernel/entry.S:581
```

在do_sys_clone返回用户态时，会触发schedule()：

```
#0  schedule ()
#1  do_notify_resume ()		at arch/arm64/kernel/signal.c:1090
#2  prepare_exit_to_user_mode ()at arch/arm64/kernel/entry-common.c:137
#3  exit_to_user_mode () 	at arch/arm64/kernel/entry-common.c:142
#4  el0_svc () 			at arch/arm64/kernel/entry-common.c:625
#5  el0t_64_sync_handler () 	at arch/arm64/kernel/entry-common.c:642
#6  el0t_64_sync () 		at arch/arm64/kernel/entry.S:581
```

**内核线程抢占**

内核抢占尽管有三种方式：

- CONFIG_PREEMPT_NONE

- CONFIG_PREEMPT_VOLUNTARY

- CONFIG_PREEMPT

实际上大多数使用的都是CONFIG_PREEMPT，当中断退出后，立即执行任务抢占。

内核线程抢占的触发点与用户线程一样，并且在触发时，也会将flag标记为RESCHED。

而抢占执行点则要比用户线程多的多，总体来说有两大类：

- 在中断执行完毕后 `preempt_schedule_irq`

- 内核代码中主动调用 `preemp_enable` 或 `schedule` 的位置

**preempt_count**

内核通过 `task_struct` -> `thread_info` -> `preempt_count` 来控制抢占的enable和disable。

- preempt_disable(): count += 1

- preempt_enable(): count -= 1 (default: 0, preemptible)

\* 这个变量还用来保存是否处在irq、softirq、nmi上下文，以及是否disable的状态。

## The Context Switch

schedule负责找一个task，切换过去继续运行

```
__schedule

	cpu_rq			# get rq

	local_irq_disable

	pick_next_task		# get next

	context_switch		# switch prev => next
```

pick_next_task负责从几个调度类中找到一个合适的task

```
pick_next_task (__pick_next_task) (CONFIG_SCHED_CORE=n)

	pick_next_task_fair	# first, get from cfs rq

	for_each_class		# then, get from other rq

		class->pick_next_task

```

context_switch负责从一个task切换到另一个task，切换后继续执行。

```
context_switch

	prepare_task_switch

	if to kernel thread
		...		# don't care about ttbr0
	elif to user thread
		__switch_mm	# switch to new ttbr0

	switch_to		# switch to new context (cpu registers)
		__switch_to
			cpu_switch_to (entry.S)

	finish_task_switch
```

之前理解有个误区，以为switch的时候所有寄存器都换，包括PC，实际context并不包含PC。

Context内容包括：

- task struct (thread info, resources)

- address space (page table)

- running stack (sp)

- cpu general registers

一个进程执行switch，现场就会停在switch这里，换进来的线程，从switch这里开始跑。

相当于PC到这里，外边的context包换掉了，PC带着新的context包继续往下跑。

切换后sp也会改变，所以相当于是回到了老task中继续执行了。

## Load Tracking

经常的，内核需要知道一个task有多大（多占用CPU），或者一个rq有多大（多busy），用来决定是否需要调频，或者task是否需要迁核，或者做负载均衡。

内核提供了`struct sched_avg`来记录对应se或者rq的负载情况，供其他调度子系统做决策使用。

关于负载这个变量的计算方式，内核历史上经过了多轮的变更，目前使用的是per-entity load tracking（PELT）的方式，高通使用window-assist load tracking（WALT）的方式，来计算负载。

这些计算方法都包括了对task处在running的时间和处在runqueue的时间的统计。在task进出队列，运行状态发生变更时，完成对task负载信息的更新。

详细描述见pelt、walt文章。

## Load Balance



## Migration

## Debug

**View task_struct from rq in GDB**

1. define offsetof
`macro define offsetof(_type, _memb)  ((long)(&((_type *)0)->_memb))`

2. define container_of
`macro define container_of(_ptr, _type, _memb) ((_type *)((void *)(_ptr) - offsetof(_type, _memb)))`

3. show task
`p *container_of((($lx_per_cpu("runqueues", 0)).cfs).curr, struct task_struct, se)`

## Items

ttwu: try to wake up

se: sched entity

## Reference

LoyenWang的系列文章：

<https://www.cnblogs.com/LoyenWang/tag/%E8%BF%9B%E7%A8%8B%E8%B0%83%E5%BA%A6/>