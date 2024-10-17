# rcu, rculist

背景：多线程下如何操作一个链表？

链表再被多线程读取时，不会有任何问题，但是当有人去改写链表时，就可能带来问题。

我们直观想到的方法是在每次操作时，给整个链表上锁。但是，对于那些大部分情况都是用于读取的链表，仅仅因为级小概率的改写，就锁定整个链表，这种方法大大影响了链表的读取效率。

RCU是内核实现的多线程操作链表的新机制，通过RCU接口及RCU的链表访问流程，可以在允许链表修改的同时，大大提高链表的读效率。

RCU机制将链表访问者分为：Reader / Updater

将链表操作分为：Insdert，Remove，Update

RCU的大体流程，就是Reader在Read时，需要通过接口进入Critical Section，Acquire掉所访问的Node，Updater去Update链表时，先将修改写入Node的一份Copy，在等到这个Node被所有Reader释放后，再完成Copy和原始Node的替换，完成更新过程。等待的这个过程，也叫做延迟释放。

进入critical section的reader，与spinlock的临界区一样，不能在临界区内睡眠或阻塞。

这种机制类似于一种注册机制，Updater作为其所更新Node的Lisener，等待作为User的Reader使用完释放Node。

实际上，RCU是一种锁机制之外的解决并发访问的通用方法，只是大部分情况下，我们只需要使用rculist这种rcu的典型应用场景。

## Files

```
- /kernel/rcu/*			// different RCU variants
- /include/linux/rcupdate.h	// RCU functional implementation
- /include/linux/rculist.h	// RCU list implementation
```

## Interface

`rcu_read_lock()`

进入read-side critical section

`rcu_read_unlock()`

退出read-side critical section

`synchronize_rcu()` / `call_rcu()`

停等或回调，在所有read-side critical section退出后，publish所有update，并（直接或者在回调中）回收内存

`rcu_assign_pointer()`

对某个pointer进行Update Copy操作，在所有read-side critical section退出后，将pointer修改为新的update copy的地址

`rcu_dereference()`

在read-side critical section中，解引用一个pointer

## Module Test

[test-rcu](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-rcu)

## Reference

<https://zhuanlan.zhihu.com/p/113999842>

<https://www.kernel.org/doc/html/latest/RCU/whatisRCU.html>

<https://lwn.net/Articles/262464/>

<https://www.kernel.org/doc/Documentation/RCU/listRCU.txt>

<https://youtube.com/watch?v=--f3V-JLHt0>

<https://youtube.com/watch?v=5964l0_BoL0>

## RCU CPU Stall

**What is CPU Stall Warning?**

CPU Stall指的时CPU停等的时间，一般为指令执行过程中的等待时间，比如等待cpu cache的访问。

CPU Stall Warning是告诉我们CPU停等的时间太久啦。(Stucked or no response for long time)

**Why RCU emits CPU Stall Warnings?**

首先，RCU不会自己产生CPU Stall Warning，产生的原因，是因为RCU内部有Stall检查机制，当Reader在Critical Section待了太久的时间(几十秒)，RCU就会报出Reader所在核出现CPU Stall Warning。意思是这个核太久没响应了。

可以通过RCU相关的boot参数或者config禁用掉这个WARNING。

**How to generate this warning**

以下code会产生CPU Stall Warning，关中断太久了，没有调度，其他核会认为你挂了。

```
local_irq_disable();
for (;;)
	do_something();
local_irq_enable();
```

类似，以下code会产生RCU CPU Stall Warning，在critical section待太久了，RCU机制会认为你挂了

```
rcu_read_lock();
for (;;)
	do_something();
rcu_read_unlock();
```

与之原理类似，还有以下情况会产生这类Warning

- Preemption disabled for long time
- bottom-half execution disable for long time
- Long running interrupt (within RCU reader)
- Fake warning caused by wrong time clock (clock or time system bug)
- Others...

当然，还有真正遇到了CPU Stall的情况，比如：

- CPU真的挂死了 (hardware issue)
- 中断风暴（interrupt overload）
- 等等...

**How to read the Warning Message**

```
INFO: rcu_sched detected stalls on CPUs/tasks:
 2-...: (3 GPs behind)    idle=06c/0/0 softirq=1453/1455 fqs=0
16-...: (0 ticks this GP) idle=81c/0/0 softirq=764/764   fqs=0
(detected by 32, t=2603 jiffies, g=7075, q=625)
```

CPU32检测到CPU2，CPU16出现RCU CPU Stall，stuck了太长时间（2603jiffies - 2.6s），2已经卡了3个grace period（延迟释放周期），这个warning之后会打印出相关CPU的线程栈。

**Supress the warnings**

RCU stall detector settings:

```
/sys/module/rcupdate/parameters/rcu_cpu_stall_timeout
```

**RCU internal debug**

enable `CONFIG_RCU_TRACE`

(待整理)

**Reference**

[Decoding RCU CPU Stall Warnings](https://youtube.com/watch?v=23_GOr8Sz-E)

[RCU CPU Stall Warnings - The Linux Kernel Archives](https://www.kernel.org/doc/Documentation/RCU/stallwarn.txt#:~:text=Please%20note%20that%20RCU%20only,the%20top%20of%20the%20stack.)

[CONFIG_RCU_TRACE debugfs Files and Formats](https://www.kernel.org/doc/Documentation/RCU/trace.txt)
