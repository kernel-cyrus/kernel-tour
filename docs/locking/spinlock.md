# spinlock

SMP系统在操作同一个变量时，需要对访问的临界区上锁，以保证操作的原子性。

spinlock是停等锁，在等锁期间，会死等。表面上，我们认为spinlock是一个死循环，实际在arm64平台实现上，spinlock的底层是一个wfe，等锁期间CPU会进入低功耗状态，在锁释放时，会通过SEV指令发送event，使wfe的核退出低功耗状态继续运行。

**锁竞争**

```
CPU 1
|
critical section (lock)
|				CPU 2
| 				|
| 				| code from another cpu
| 				|
|
| |
| | preempt (thread B)
| |
|
|
| |
| | soft irq
| |
| | |
| | | interrupt
| | |
| |
|
critical section (unlock)
|
```

spinlock需要考虑的核心问题，是锁竞争问题，即一个锁lock后，不可以再spinlock一次，由于spinlock是死等的，重锁会导致死锁。

所以解决这个问题需要思考的是当一把锁lock后，代码进入临界区后，除了临界区内自身的代码外，什么样的情况可能导致这个锁被重新lock。

情况1：多核并行

多核lock同一把锁不会有任何问题，就是锁的最正常场景

情况2：临界区-抢占

如果在不同进程上下文中申请同一把锁，那么如果临界区发生了进程抢占，就可能会死锁。所以所有的spin_lock接口都会禁用抢占，所以这种多进程上下文lock同一把锁也是spinlock的通用情况，也不会存在问题。

情况3：临界区-软中断

由于spinlock不一定会关闭中断（关闭中断是一个比较大开销的动作），如果确定没有在中断里申请同一把锁，就不需要关中断，如果不能保证中断中会发生申请同一把锁的情况，就需要关毕本CPU的local中断（其他CPU中断内抢锁输入多核并行的通用情况，并不会引起任何问题）。

情况4：临界区-硬中断

与情况3一样，软中断可以被硬中断继续中断。

所以按照上面机种情况，一个CPU上，由于抢占或者中断的嵌套，可能同时申请到三把锁。

**critical section**

临界区内的代码，需要注意的是进入临界区的状态。

比如已经preempt disable的情况下，就不能再让出调度，否则就无法再调度回来。

**spinlock, raw_spinlock**

raw_spinlock的历史，主要源于spinlock在CONFIG_PREEMPT_RT特性的和入，RT特性允许将linux作为实时操作系统使用。

RT模式下，spin_lock被修改为了可睡眠锁，但是有些spinlock锁在内核位置，在RT模式下也不允许抢占核睡眠，就将这些绝对需要禁止抢占核睡眠的spinlock写做raw_spin_lock

在非RT模式下，这两个锁的实现是完全一致的，一般情况下使用spin_lock即可，在绝对不允许被抢占和睡眠的地方，使用raw_spin_lock。

**rw_lock**

rw_lock如果没有特别原因，已经被rcu、rcu-list取代。

**spinlock使用场景**

spin_lock 使用场景
首先如果整个临界区都只位于进程上下文或者工作队列中，那么只需要采用最为方便的 spin_lock 即可，因为他不会发生中断抢占锁的情况，哪怕中断抢占进程上下文也不会导致中断由于申请自旋锁而导致死锁。

还有一种情况就是在硬件中断中可以考虑使用 spin_lock 即可，因为硬件中断不存在嵌套（未必一定是这样，与平台有关），所以只需要简单的上锁即可， 可以不需要关闭中断，保存堆栈等。

spin_lock_irq 使用场景
这个锁的变种适合在进程上下文/软中断 + 硬件中断这样的组合中使用，taskset 也是属于软中断的一种，所以也归在此类。当然，这种类型的变种同样适合软中断/taskset + 进程上下文的组合，因为关闭了硬件中断，从源头就禁止执行软中断代码，不过，对于这种类型的中断最好的方式是使用 spin_lock_bh 的方式，因为他只锁定软中断代码执行，而不关闭硬件中断，这样性能损耗更小。

spin_lock_irqsave 使用场景
这种类型的使用方式是最为安全以及便捷的，毕竟不需要考虑会不会发生死锁的问题（代码本身引入的死锁不在此类），但是他也是性能损耗最大的代码，能不使用尽量不适用，在高速设备上，自旋锁已然成为了一种降低性能的瓶颈。他最好只出现在在需要尝试 spin_lock 之前无法确定是否已经关闭中断的代码才使用，如果代码能够确定在执行锁之前中断一定是打开的，那么使用 spin_lock_irq 是更佳的选择。

spin_lock_bh 使用场景
这种类型的变种是一种比 spin_lock_irq 更轻量的变种，只关闭中断底半部，其实就是关闭了软中断、Taskset 以及 Timer 等的一个抢占能力，如果开发者确定编写的代码临界区只存在软中断/Taskset/Timer + 进程上下文这样的组合，则最好考虑使用 spin_lock_bh 这样的锁来禁止软中断进行抢占。还有就是软中断与软中断自我抢占临界区访问时，也需要使用 spin_lock_bh 以上的中断锁，因为有可能软中断在执行的过程中，自己被硬件中断打断，然后又执行到同样的代码，在别的 CPU 执行还好说，毕竟软中断可以在不同的 CPU 上执行同一个中断函数，但是假设不幸运行在同一个 CPU 上，则会导致死锁。Taskset 由于在运行过程中钟只会运行一个实例，所以不存在死锁问题，Taskset 与 Taskset 的锁竞争只需要使用 spin_lock 即可。

## Files

```
- /include/linux/spinlock.h
- /kernel/locking/spinlock.c
```

## Interface

`DEFINE_SPINLOCK(name)`

定义一把锁

`spin_lock`, `spin_unlock`

获取锁，禁用抢占。

`spin_lock_bh`, `spin_unlock_bh`

获取锁，禁用软中断（下半部：tasklet、softirq）

`spin_lock_irq`, `spin_unlock_irq`

获取锁，禁用硬中断。这个禁用或开启不支持嵌套，就是单纯的disable/enable

`spin_lock_irqsave`, `spin_unlock_irqrestore`

获取锁，禁用硬中断。支持嵌套。

`spin_try_lock`

获取锁，如果锁被占用则返回失败。

`spin_is_locked`

检查一把锁是否被占用。

## Debug Test

CONFIG_INLINE_SPIN_LOCK

## Reference

<https://www.bbsmax.com/A/6pdD4O3Rdw/>

<https://zhuanlan.zhihu.com/p/90634198>

<https://www.byteisland.com/%E8%87%AA%E6%97%8B%E9%94%81-spin_lock%E3%80%81-spin_lock_irq-%E4%BB%A5%E5%8F%8A-spin_lock_irqsave-%E7%9A%84%E5%8C%BA%E5%88%AB/>