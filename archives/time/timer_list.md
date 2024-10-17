# timer_list

`/proc/timer_list` 节点可以dump出当前系统中的hrtimer和tick device。

代码实现在 `kernel/time/timer_list.c` 中。

主要包括三部分信息：

1、 每个CPU的hrtimer base，每个base的clock base，每个clock base的timerqueue
2、 全局tick broadcast device
3、 每个CPU的tick device

其中，数据结构详情见hrtimer_bases全局变量。

```
Timer List Version: v0.9
HRTIMER_MAX_CLOCK_BASES: 8			# clock base for each hrtimer base
now at 47959154304 nsecs			# boot time (CLOCK_MONOTONIC)

================= Per CPU hrtimer base / clock base / timerqueue =================

cpu: 0						# CPU
 clock 0:					# clock base 0
  .base:       00000000bfe65cc8			# clock base variable address pointer
  .index:      0				# internal enum index
  .resolution: 1 nsecs				# resolution
  .get_time:   ktime_get			# clock get function
  .offset:     0 nsecs
active timers:					# timers in clock base 0
 #0: <0000000006828050>, tick_sched_timer, S:01	# timer 0, handler, expires
 # expires at 47964000000-47964000000 nsecs [in 4845696 to 4845696 nsecs]
 #1: <00000000808cda1d>, sched_clock_poll, S:01	# timer 1, handler, expires
 # expires at 4398054511096-4398054511096 nsecs [in 4350095356792 to 4350095356792 nsecs]
 --------------- other clock base ---------------
 clock 1:
  .base:       00000000522d7a38
  .index:      1
  .resolution: 1 nsecs
  .get_time:   ktime_get_real
  .offset:     1667996150000023072 nsecs
active timers:
 clock 2:
  .base:       0000000088d84c6c
  .index:      2
  .resolution: 1 nsecs
  .get_time:   ktime_get_boottime
  .offset:     0 nsecs
active timers:
 clock 3:
  .base:       00000000ed610d9f
  .index:      3
  .resolution: 1 nsecs
  .get_time:   ktime_get_clocktai
  .offset:     1667996150000023072 nsecs
active timers:
 clock 4:
  .base:       0000000049b42597
  .index:      4
  .resolution: 1 nsecs
  .get_time:   ktime_get
  .offset:     0 nsecs
active timers:
 clock 5:
  .base:       00000000f2bc6e99
  .index:      5
  .resolution: 1 nsecs
  .get_time:   ktime_get_real
  .offset:     1667996150000023072 nsecs
active timers:
 clock 6:
  .base:       000000002a6bd0e6
  .index:      6
  .resolution: 1 nsecs
  .get_time:   ktime_get_boottime
  .offset:     0 nsecs
active timers:
 clock 7:
  .base:       000000005c04e6b8
  .index:      7
  .resolution: 1 nsecs
  .get_time:   ktime_get_clocktai
  .offset:     1667996150000023072 nsecs
active timers:
  .expires_next   : 125916000000 nsecs
  .hres_active    : 1
  .nr_events      : 831
  .nr_retries     : 0
  .nr_hangs       : 0
  .max_hang_time  : 0
  .nohz_mode      : 2
  .last_tick      : 47964000000 nsecs
  .tick_stopped   : 1
  .idle_jiffies   : 4294904287
  .idle_calls     : 537
  .idle_sleeps    : 305
  .idle_entrytime : 47960627648 nsecs
  .idle_waketime  : 47959759056 nsecs
  .idle_exittime  : 47959778576 nsecs
  .idle_sleeptime : 45019336528 nsecs
  .iowait_sleeptime: 0 nsecs
  .last_jiffies   : 4294904287
  .next_timer     : 125916000000
  .idle_expires   : 125916000000 nsecs
 --------------- other clock base end ---------------
jiffies: 4294904287				# jiffies

<... Other CPU ...>

================= Tick Broadcast Device =================

Tick Device: mode:     1
Broadcast device
Clock Event Device: bc_hrtimer
 max_delta_ns:   9223372036854775807
 min_delta_ns:   1
 mult:           1
 shift:          0
 mode:           1
 next_event:     9223372036854775807 nsecs
 set_next_event: 0x0
 shutdown:       bc_shutdown
 event_handler:  tick_handle_oneshot_broadcast

 retries:        0

tick_broadcast_mask: 0
tick_broadcast_oneshot_mask: 0

================= Per CPU tick device =================

Tick Device: mode:     1				# is on
Per CPU device: 0					# cpu
Clock Event Device: arch_sys_timer			# clock event device
 max_delta_ns:   2199023255551
 min_delta_ns:   1000
 mult:           8388608
 shift:          27
 mode:           3
 next_event:     125916000000 nsecs			# next event time
 set_next_event: arch_timer_set_next_event_virt
 shutdown:       arch_timer_shutdown_virt
 oneshot stopped: arch_timer_shutdown_virt
 event_handler:  hrtimer_interrupt			# tick device handler

 retries:        0
Wakeup Device: <NULL>

<... Other CPU ...>

```