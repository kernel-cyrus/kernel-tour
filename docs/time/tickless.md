# tickless(nohz)

peroid tick会按照固定周期触发到期中断，中断或者唤醒cpu来处理tick流程（处理timer、处理调度等等）。如果没有到期timer需要处理，或者没有任务需要切换，那么这个tick中断实际是可以避免的，从而节省功耗，并提高性能。

**periodic / nohz idle / nohz full**

tick有三种工作模式：periodic，nohz idle，nohz full

1、periodic（CONFIG_NO_HZ=n，CONFIG_HZ_PERIODIC=y）

周期性tick。（Old Kernel）

2、nohz idle（CONFIG_NO_HZ=y，CONFIG_NO_HZ_IDLE=y）

CPU idle时不产生tick。（默认，本文主要关心的模式）

"dyntick-idle", "in dyntick-idle mode", "in nohz mode", or "running tickless" 都指这种模式

在CONFIG_NO_HZ_IDLE开启的情况下，还可以通过nohz=on/off在bootargs中动态开关dyntick。

3、nohz full（CONFIG_NO_HZ=y，CONFIG_NO_HZ_FULL=y）

CPU idle、CPU只有一个任务运行时，不产生tick。（一般不使用）

被设置为nohz full的CPU叫做"adaptive-ticks CPUs"。

可以通过bootargs nohz_full=1,6-8来设置启用nohz full的CPU。（不能将cpu0设置为nohz full）

> By default, without passing the nohz_full parameter, this behaves just like NO_HZ_IDLE.

**periodic vs nohz**

not trigger ticks when idle.（"|" is a tick）

- tick_handle_periodic

```
|    T1    |    T2    |    IDLE    |    IDLE    |    T3    |
```

- tick_nohz_handler（CONFIG_NOHZ_IDLE）

```
|    T1    |    T2    |          IDLE           |    T3    |
```

**high resolution mode**

allow timer event inside a tick, which means you can fire a timer event anytime.（"^" is a timer event）

- tick_nohz_handler

```
|    T1    |    T2    |^         IDLE           |^   T3    |
```

- hrtimer_interrupt

```
|    T1    |   T ^ 2  |^         IDLE     ^  ^  |^   T3    |
```

## Macros

```
TICK_ONESHOT		# build nohz related source files
NO_HZ_COMMON		# nohz common framework
NO_HZ_IDLE		# nohz idle feature
NO_HZ_FULL		# nohz full feature
```

## Files

```
- /kernel/time/tick-sched.c
```

## Structures

`struct tick_sched`

## Variables

`tick_cpu_sched`

Percpu tick_sched 全局变量。