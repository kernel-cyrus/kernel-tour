# softirq

软中断是为了解决在中断响应过程中，过长屏蔽中断导致中断响应不及时甚至发生丢中断问题而引入的机制。软中断使用中断上下文，在中断回调函数执行后执行，在软中断执行函数__do_softirq中，会重新开启中断，允许被打断，所以尽管软中断使用中断上下文，但可以被中断打断。软中断的本质是开了中断的中断上下文，他的处理高于任何任务优先级。

softirq会在两个地方被fire，一个是在硬中断执行结束后，执行那些在硬中断中raise的软中断；一个是在local_bh_enable()/spin_unlock_bh()，也就是下半部使能时，这部分的执行具有一定的ramdom属性（执行时机不那么可预测）。

实现上，所有软中断及回调函数，以优先级为索引，被定义在一个全局数组中，各类使用软中断的机制，会在自己的中断处理函数中raise其所定义的软中断，被raise的软中断会在中断处理结束后被执行。同时，软中断会在每个cpu创建一个softirq（ksoftirqd）线程，允许没有处理的软中断在线程中被执行。

软中断的申请在enum中静态定义，不能动态申请。系统中的软中段都是各类机制所特权使用的（允许代码被插入到中断处理结束后执行）。所以基本上，我们只会使用基于软中断机制实现的tasklet，来在中断后半部插入自己代码，而不会定义新的静态软中断类型。

软中断在多核上是并行的，所以软中断回调函数都是会被重入的（包括多核重入和中断重入），必须用spinlock来保护临界区。

## Files

```
- /kernel/softirq.c
```

## Variables

`softirq_vec[NR_SOFTIRQS]`

保存了所有softirq的回调函数，其中NR_SOFTIRQS所在的enum定义了全部softirq类型，以优先级作为索引。

`softirq_to_name[NR_SOFTIRQS]`

所有softirq类型对应的名字字符串，包括 "HI", "TIMER", "NET_TX", "NET_RX", "BLOCK", "IRQ_POLL", "TASKLET", "SCHED", "HRTIMER", "RCU"

`softirq_threads`

```
static struct smp_hotplug_thread softirq_threads = {
	.store			= &ksoftirqd,			// percpu task struct
	.thread_should_run	= ksoftirqd_should_run,		// check if thread should run
	.thread_fn		= run_ksoftirqd,		// thread function 
								// (the loop is in smp wrapper smpboot_thread_fn)
	.thread_comm		= "ksoftirqd/%u",		// kthread name
};
```

定义了percpu线程（使用smp_hotplug_thread）

smp会为每个核创建出percpu线程，并绑定自己的线程函数 `smpboot_thread_fn`，在这个wrapper函数的loop中，会检查 `thread_should_run`，只有返回true时（有pending softirq），才会调用 `run_ksoftirqd` 进行处理。

`run_ksoftirqd` 会根据优先级遍历 `softirq_vec`，并调用各个softirq的回调函数来处理掉pending的softirq。

`local_softirq_pending_ref`

percpu pending变量，记录当前核的pending softirq。这个变量提供了一组操作接口。

## Functions

`open_softirq`

将回调函数绑定到某路softirq上

`raise_softirq`

rasie一个softirq。会将当前cpu对应的softirq pending置位，然后唤醒当前cpu的softirq线程。

softirq最终可能在三个位置得到处理，一个是硬中断处理函数退出时（irq_exit_rcu中直接调用invloke_softirq），一个是local_bh_enable时，最后是在softirq线程被调度时。

`__do_softirq`

softirq最终处理函数，下面三个函数都会最终使用这个函数来处理softirq。

softirq会直接使能中断，这是软中断可以被硬中断打断的原因。

`run_ksoftirqd`

softirq线程函数

`invoke_softirq`

直接处理softirq，在中断退出时被调用。

`local_bh_enable`, `local_bh_disable`

禁用/使能softirq

`irq_enter`, `irq_exit`

硬中断处理前后的插桩函数，softirq在exit中被执行

`irq_enter_rcu`, `irq_exit_rcu`

硬中断处理前后的插桩函数，softirq在exit中被执行

```
static void __el1_irq()
{
	...
	irq_enter_rcu();
	do_interrupt_handler(regs, handler);	<- 中断回调执行
	irq_exit_rcu();				<- 软中断执行
	...
}
```

## Reference

<https://www.oreilly.com/library/view/understanding-the-linux/0596002130/ch04s07.html>

<https://blog.csdn.net/oqqYuJi12345678/article/details/99771140>
