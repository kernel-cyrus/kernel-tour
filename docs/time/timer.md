# timer

低精度timer，如其名，是不保证精度的timer。

低精度timer主要提供了msleep、ssleep、usleep_range等接口，用来做毫秒级定时。

低精度timer基于jiffies的，也就是说，其内部的定时和触发都是使用jiffies来计数的。timer在tick device触发的tick中断的后半部分（softirq）中执行。

timer的管理，在设计时主要要考虑三个问题，第一是插入timer时算法复杂度足够低，第二是到期时读取待triger的timer复杂度足够低，第三是尽量合并处理，避免产生过多的中断。

timer使用一个哈希链表来保存所有注册进来的timer。

```
 * HZ  250
 * Level Offset  Granularity            Range
 *  0	   0         4 ms                0 ms -        255 ms
 *  1	  64        32 ms              256 ms -       2047 ms (256ms - ~2s)
 *  2	 128       256 ms             2048 ms -      16383 ms (~2s - ~16s)
 *  3	 192      2048 ms (~2s)      16384 ms -     131071 ms (~16s - ~2m)
 *  4	 256     16384 ms (~16s)    131072 ms -    1048575 ms (~2m - ~17m)
 *  5	 320    131072 ms (~2m)    1048576 ms -    8388607 ms (~17m - ~2h)
 *  6	 384   1048576 ms (~17m)   8388608 ms -   67108863 ms (~2h - ~18h)
 *  7	 448   8388608 ms (~2h)   67108864 ms -  536870911 ms (~18h - ~6d)
 *  8    512  67108864 ms (~18h) 536870912 ms - 4294967288 ms (~6d - ~49d)
```

以HZ=250为例，一个tick（jiffies）时间为4ms，在timer_base中，会使用512个hlist（vector\[WHEEL_SIZE\]）来保存不同到期时间的timer list。上表中的Offset为idx，Level为该index对应的level（不同level有不同的颗粒度granularity）。Granularity后面的括号是这个粒度下的最大误差。Range是这个Level下的index所能挂的全部timer范围。

当timer被插入时，会先计算到期时间delta，delta等于到期jiffies-当前jiffies，然后使用delta计算出应该放进哪个LEVEL的vector，在LEVEL中，在通过expiry（目标到期jiffies）在这个level的索引MASK，计算出内部偏移INDEX。最终的vector index等于LEVEL offset + LEVEL内部偏移INDEX。

在timer到期时，会使用CLK（当前jiffies）遍历每个LEVEL，遍历过程中，通过CLK和对应LEVEL的MASK，获得每个LEVEL的到期vector，然后运行这些vector中的timer。这就达到了插入和检出时，都能达到O(1)复杂度。

**cascade vs non-cascade**

早期的低精度timer设计，在一个timer到期时，会不断把高LEVEL向低LEVEL做迁移，使所有timer都能达到比较高的精度。

https://lore.kernel.org/lkml/20160617131003.810514984@linutronix.de/

提到，在现实数据中，大部分timer都会在到期前取消或者重新修改，并且大部分timer都不会休眠几天这么久。所以这种为了长时间timer的精度而做的迁移工作，就显得没有什么意义。

所以这个patch将timer彻底转移为了non-cascade timer，不再在每次timer到期时重新迁移timer，而就让他们保持低精度。

这带来的好处就是，第一，短时间的timer，仍然可以有比较高的精度，第二，长时间的timer，即使迁移，也会有比较大的误差，那就让他保持低精度，第三，避免了频繁的迁移那些大部分会被取消的timer。

## Files

```
- /kernel/time/timer.c		# 低精度timer机制实现
- /include/linux/delay.h	# timer and delay interface
```

## Structures

```
struct timer_list {
	struct hlist_node	entry;		# 用来挂到timer_base->vector下
	unsigned long		expires;	# 到期jiffies（jiffies等于这个值触发）
	void			(*function)(struct timer_list *);	# timer回调
	u32			flags;
};
```

```
struct timer_base {
	raw_spinlock_t		lock;
	struct timer_list	*running_timer;		# 当前正在运行的timer，timer运行时被从vector取出放到这里
	unsigned long		clk;			# 一般就是当前jiffies
	unsigned long		next_expiry;		# 下次触发的jiffies
	unsigned int		cpu;
	bool			next_expiry_recalc;
	bool			is_idle;
	bool			timers_pending;		# 是否有timer注册进来
	DECLARE_BITMAP(pending_map, WHEEL_SIZE);	# vecotor的not empty bitmap
	struct hlist_head	vectors[WHEEL_SIZE];	# 512个hlist，挂不同到期时间的timer
} ____cacheline_aligned;
```

## Variables

`jiffies_64`

jiffies

`timer_base`

percpu timer_base，每个核上的timer管理全局变量。

## Functions

`init_timers`

初始化timer_base，初始化timer使用的软中断。

**Timer 使用**

`msleep`

休眠ms，通过ms计算出目标jiffies，添加到对应vector

`schedule_timeout`

`schedule_timeout_interruptible`

`schedule_timeout_killable`

`schedule_timeout_uninterruptible`

**Timer 添加**

`internal_add_timer`

通过 `calc_wheel_index` 计算出目标vector的index，把timer enqueue进去。

**Timer 到期**

timer到期执行分两部:

第一部分是在tick device的tick handler里（tick_handle_periodic / tick_periodic / update_process_times / run_local_timers）raise TIMER_SOFTIRQ

第二部分是在下半部，执行timer softirq，处理所有到期timer。

`run_timer_softirq`

timer软中断处理函数，会调用 `__run_timers`，通过当前jiffies(timer_base->clk)，使用 `collect_expired_timers` 取出每个level到期的vector，然后执行这些vector中的timer。

**hres后**

切换hres模式后，原有的tick handler被hrtimer_interrupt接管，tick被sched_timer取代，原有在tick handler中执行的update_process_times，转到sched timer的tick_sched_handle中被调用。第一部分的raise softirq的动作在这里触发。在中断处理的下半部，执行run_timer_softirq处理到期的低精度timer

## Debugfs

`CONFIG_DEBUG_OBJECTS_TIMERS`

使能timer生命周期debug，会track timer的init、alloc、free，如果发现问题则报error。

commit c6f3a97f86a5c97be0ca255976110bb9c3cfe669

**关于Object生命周期Debug**

<https://www.kernel.org/doc/html/latest/core-api/debug-objects.html>

这个机制非常适用于，kernel提供机制，对用户提供init、alloc、free接口后，对用户使用行为的BUG预防检测。