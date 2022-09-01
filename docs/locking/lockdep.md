# lockdep

lockdep是内核引入的死锁检测机制，只需开启这一特性，lockdep会自动在锁初始化时记录这个锁的类型（每个lock代码初始化位置对应一个锁类，key），在申请和释放时会更新内部锁关系数据链表，通过内部的死锁检测算法，在发生死锁时打印出死锁原因。

关注这一特性，不需要过多关注内部算法实现，更多时候，我们需要知道如何使用他，如何阅读lockdep信息。

## Files

```
- /include/linux/lockdep.h
- /kernel/locking/lockdep.c
```

## Implementation

初始化

```
 spin_lock_init()
            ↓
    raw_spin_lock_init()
            ↓
    __raw_spin_lock_init()
            → debug_check_no_locks_freed()
            → lockdep_init_map()
            → 初始化 spin_lock 的值

# define raw_spin_lock_init(lock)			\
    do {						\
    	static struct lock_class_key __key;		\
    	__raw_spin_lock_init((lock), #lock, &__key);	\
    } while (0)
```

锁申请

```
 spin_lock()
        ↓
    raw_spin_lock()
        ↓
    _raw_spin_lock() @kernel/spinlock.c
        ↓
    __raw_spin_lock() @include/linux/spinlock_api_smp.h
        → preempt_disable();
        → spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);
                ↓
            lock_acquire() → __lock_acquire() → __lock_acquire()
            __lock_acquire() 是 lockdep 死锁检测的核心，所有原理中描述的死锁错误都是在这里检测的。如果出错，最终会调用 print_xxx_bug() 函数。
        → LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
```

## Usage Test

**Enable lockdep with menu config**

```
[*] Detect Hard and Soft Lockups
[*] Detect Hung Tasks
[*] RT Mutex debugging, deadlock detection
-*- Spinlock and rw-lock debugging: basic checks
-*- Mutex debugging: basic checks
-*- Lock debugging: detect incorrect freeing of live locks
[*] Lock debugging: prove locking correctness
[*] Lock usage statistics
```

**Enable lockdep with defconfig**

```
// This feature enables the kernel to report locking related deadlocks before they actually occur. For more details, see Documentation/locking/lockdep-design.txt.
CONFIG_PROVE_LOCKING=y

// Detect incorrect freeing of live locks.
CONFIG_DEBUG_LOCK_ALLOC=y

// The lock dependency engine will do additional runtime checks to debug itself, at the price of more runtime overhead.
CONFIG_DEBUG_LOCKDEP=y

// Lock usage statistics. For more details, see Documentation/locking/lockstat.txt
CONFIG_LOCK_STAT=y

// The kernel to run a short self-test during bootup in start_kernel(). The self-test checks whether common types of locking bugs are detected by debugging mechanisms or not. For more details, see lib/locking-selftest.c
CONFIG_DEBUG_LOCKING_API_SELFTESTS=y

CONFIG_LOCKUP_DETECTOR=y
CONFIG_HARDLOCKUP_DETECTOR_OTHER_CPU=y
CONFIG_HARDLOCKUP_DETECTOR=y
CONFIG_DEBUG_ATOMIC_SLEEP=y
```

**Check lockdep info**

```
/proc/lockdep
/proc/lockdep_chains
/proc/lockdep_stat
/proc/locks
/proc/lock_stats
```

**Check lockdep warnings**

如果发生deadlock，会打出warning和典型场景图帮助分析。

## Knowledge

**Lock status**

锁类有 4n + 1 种状态：

```
- 'ever held in STATE context'
- 'ever held as readlock in STATE context'
- 'ever held with STATE enabled'
- 'ever held as readlock with STATE enabled'
*
- hardirq context
- softirq context
+
- 'ever used' [ == !unused        ]
```

比如：

```
(&sio_locks[i].lock){-.-.}, at: [<c02867fd>] mutex_lock+0x21/0x24
                     ||||
                     ||| \-> softirq disabled and not in softirq context	(softirq read lock)
                     || \--> acquired in softirq context			(softirq)
                     | \---> hardirq disabled and not in hardirq context	(hardirq read lock)
                      \----> acquired in hardirq context			(hardirq)
```

各个字符代表的含义分别如下：

```
‘.’ acquired while irqs disabled and not in irq context
‘-’ acquired in irq context
‘+’ acquired with irqs enabled
‘?’ acquired in irq context with irqs enabled.
```

或者用矩阵表示：

```
		irq enabled	irq disabled
ever in irq	‘?’		‘-’
never in irq	‘+’		‘.’
```

（还是不太理解。）

**Deplock checking rules**

单锁检测规则：

一个锁，一定不可能有的地方用lock_irqsave（关中断），有的地方用lock（不关中断），见locking/spinlock，那么一旦发现这类冲突，则会报出单锁检测的ERROR。

下面四个定义，正好对应了四个锁函数。

- (1) ever held in hard interrupt context (hardirq-safe);
- (2) ever held in soft interrupt context (softirg-safe);
- (3) ever held in hard interrupt with interrupts enabled (hardirq-unsafe);
- (4) ever held with soft interrupts and hard interrupts enabled (softirq-unsafe);
- (5) ever used (!unused).

多锁依赖规则：

就是锁间依赖关系的检测算法

**Typical warnings**

```
[ INFO: possible circular locking dependency detected ] // 圆形锁，获取锁的顺序异常（ABBA）
[ INFO: %s-safe -> %s-unsafe lock order detected ] // 获取从 safe 的锁类到 unsafe 的锁类的操作
[ INFO: possible recursive locking detected ] // 重复去获取同类锁（AA）
[ INFO: inconsistent lock state ] // 锁的状态前后不一致
[ INFO: possible irq lock inversion dependency detected ] // 嵌套获取锁的状态前后需要保持一致，即 [hardirq-safe] -> [hardirq-unsafe]，[softirq-safe] -> [softirq-unsafe] 会警报死锁风险
[ INFO: suspicious RCU usage. ] // 可疑的 RCU 用法
```

## Reference

<https://www.kernel.org/doc/html/latest/locking/lockdep-design.html>

<https://lwn.net/Articles/185666/>

<http://www.lenky.info/archives/2013/04/2253>

<http://kernel.meizu.com/linux-dead-lock-detect-lockdep.html>