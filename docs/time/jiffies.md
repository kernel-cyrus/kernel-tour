**jiffies**

CONFIG_HZ=250（每秒钟250个tick）

vmlinux.lds.S
jiffies=jiffies_64

timer.c
jiffies_64 = INITIAL_JIFFIES


jiffies.h
INITIAL_JIFFIES is -300\*HZ

-------------------

Searching 39355 files for "jiffies_64 +=" (case sensitive)

/home/cyrus/Workspace/codebase/linux-kernel/mainline/kernel/time/tick-sched.c:
  116  
  117  	/* Advance jiffies to complete the jiffies_seq protected job */
  118: 	jiffies_64 += ticks;
  119  
  120  	/*

/home/cyrus/Workspace/codebase/linux-kernel/mainline/kernel/time/timekeeping.c:
 2286  void do_timer(unsigned long ticks)
 2287  {
 2288: 	jiffies_64 += ticks;
 2289  	calc_global_load();
 2290  }

2 matches across 2 files

--------------------

jiffies clocksource

default clocksource