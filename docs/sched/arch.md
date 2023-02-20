# sched architecture

From the very begining of scheduling

## Data Structures

![Data Structures]()

**Task**

`struct task_struct`

`struct task_group`

**Entity**

`struct sched_entity`

`struct sched_dl_entity`

`struct sched_rt_entity`

**Runqueue**

`struct rq`

`struct cfs_rq`

`struct dl_rq`

`struct rt_rq`

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

-------------------

```

# for cgroup task scheduling

list: task_groups			# all task groups

struct task_group root_task_group;	# link to task_groups list
	list -> task_groups
	children
	siblings

# percpu run queue

runqueues (percpu) thread that can run! where pick_next get from!
	cfs
	rt
	dl
	*idle
	*stop
	*curr
	nr_running
	nr_uninterruptble
	nr_switches

sched_entity	# object in a rq, embeded in task_struct or task_group
	parent	# for group scheduling
	# means a task or a task_group (group of tasks)

task_struct
	......
	pid
	comm			# name
	thread_intfo
		ttbr0_el1
	thread_struct		# tcb
		cpu context
	sched_entity
	sched_rt_entity
	sched_dl_entity
	sched_task_group	# group belongs to
	list
	parent
	children
	siblings
	resources...

struct task_struct init_task;

```


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

Sleep，Preempt，Interrupt

## The Context Switch

## Load Balance

## Migration

## Load Tracking

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