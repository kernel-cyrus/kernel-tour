# jiffies

**What is jiffies**

jiffies就是tick的counter，每个tick => jiffies+1。

tick频率在config中可以设置，默认情况下，CONFIG_HZ=250（每秒钟250个tick）

jiffies在vmlinux.lds.S中定义，实际指向了jiffies_64，在timer.c中定义。

```
vmlinux.lds.S:
	jiffies=jiffies_64

timer.c
	jiffies_64 = INITIAL_JIFFIES

jiffies.h
	INITIAL_JIFFIES is -300\*HZ
```

**How jiffies update**

jiffes在tick触发时更新，所以它的更新依赖于tick device，而tick device则依赖于硬件arch timer的初始化。所以在arch timer初始化之前，在timer interrupt能够产生前，或者说在tick device被创建前，jiffies是不会被更新的。

在arch timer初始化后，legacy timer开始工作，jiffies在tick_handle_periodic中被更新。

在hrtimer初始化后，tick切换到high res模式，hrtimer interrupt handler接管了原来的tick handler，tick被重新实现成了一个hrtimer(sched_timer)，这之后jiffies在sched timer中更新。

```
Searching 39355 files for "jiffies_64 +=" (case sensitive)

# Jiffies update in high res mode
/home/cyrus/Workspace/codebase/linux-kernel/mainline/kernel/time/tick-sched.c:
  116  
  117  	/* Advance jiffies to complete the jiffies_seq protected job */
  118: 	jiffies_64 += ticks;
  119  
  120  	/*

# Jiffies update in legacy mode
/home/cyrus/Workspace/codebase/linux-kernel/mainline/kernel/time/timekeeping.c:
 2286  void do_timer(unsigned long ticks)
 2287  {
 2288: 	jiffies_64 += ticks;
 2289  	calc_global_load();
 2290  }

2 matches across 2 files
```

**jiffies clocksource**

jiffies在能够被周期更新的基础上，被实现为一个clock source，作为timekeeping默认的clocksource，为timekeeping提供低精度服务。

在arch timer clocksource注册后，timekeeping切换到精度更高的arch timer clocksource。

在启动log中，可以看到这个切换过程：

```
# dmesg | grep clocksource
[    0.000000] clocksource: arch_sys_counter: mask: 0x1ffffffffffffff max_cycles: 0x1cd42e208c, max_idle_ns: 881590405314 ns
[    0.140935] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 7645041785100000 ns
[    0.385832] clocksource: Switched to clocksource arch_sys_counter
```