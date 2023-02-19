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

`task_struct init_task` (init_task.c)

swapper

`__sched_fork`

secondary cpu: copy of init_task

```
#0  init_idle (idle=idle@entry=0xffff000003133700, cpu=cpu@entry=1)
    at kernel/sched/core.c:8907
#1  0xffff8000098f9388 in fork_idle (cpu=cpu@entry=1) at kernel/fork.c:2573
#2  0xffff8000098fb23c in idle_init (cpu=1) at kernel/smpboot.c:55
#3  idle_threads_init () at kernel/smpboot.c:74
#4  0xffff8000099008b8 in smp_init () at kernel/smp.c:1106
#5  0xffff8000098f14a8 in kernel_init_freeable () at init/main.c:1602
#6  0xffff800008fa7f94 in kernel_init (unused=<optimized out>) at init/main.c:1499
#7  0xffff8000080159b8 in ret_from_fork () at arch/arm64/kernel/entry.S:867

```

all init task finally run into idle.

## Process Lifecycle

## Priority and Schedule Class

## Tick Wheels

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