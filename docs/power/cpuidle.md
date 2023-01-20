# cpuidle

CPU Idle的实现分为四层：sched，idle device，arm idle driver，idle governor。

**Sched Idle**

Idle调度类，优先级最低，在所有rq都没有可运行任务时，进入Idle，在sched中详细描述。

**Idle Device**

Idle Device框架，具体实现在/drivers/cpuidle/cpuidle.c中。

定义了cpuidle_driver，cpuidle_device及注册，提供出`cpuidle_enter`接口及cpu_idle device的管理控制接口。

ARM基于Idle Device框架，实现了自己的Idle Device Driver（/drivers/cpuidle/cpuidle-arm.c）

在device_initcall阶段，调用arm_idle_init，根据dts中配置的"arm,idle-state"，为每个CPU初始化arm idle driver，并注册到idle device框架中。

这些state在driver中定义，在dts中动态可配。比如driver支持3中state，但是某些核只支持两种。

Idle Devce框架包括了Percpu的idle_device，静态定义，在idle driver注册时被初始化。

同时还包括Percpu的cpuidle_devices（idle device指针），指向默认静态定义的cpuidle_dev。

**ARM Driver**

ARM Driver的底层主要实现了arm_enter_idle_state

在进入时判断state的index，默认为wfi

如果index存在的话，会进入arm_cpuidle_suspend，最终通过cpu_ops->suspend调用psci进入ATF执行suspend流程。

**do_idle()**

CPU Idle的入口函数是`do_idle`，进入路径一共有三个：

1、Primary CPU

```
#0  do_idle () 			at kernel/sched/idle.c:259
#1  cpu_startup_entry 		at kernel/sched/idle.c:400
#2  rest_init () 		at init/main.c:726
#3  arch_call_rest_init () 	at init/main.c:882
#4  start_kernel () 		at init/main.c:1137
#5  __primary_switched () 	at arch/arm64/kernel/head.S:468
```

2、Secondary CPU

```
#0  do_idle () 			at kernel/sched/idle.c:259
#1  cpu_startup_entry 		at kernel/sched/idle.c:400
#2  secondary_start_kernel () 	at arch/arm64/kernel/smp.c:265
#3  __secondary_switched () 	at arch/arm64/kernel/head.S:661
```

3、Schedule to IDLE

```
#0  pick_next_task_idle 	at kernel/sched/idle.c:448
#1  __pick_next_task     	at kernel/sched/core.c:5775
#2  pick_next_task 	 	at kernel/sched/core.c:6277
#3  __schedule 			at kernel/sched/core.c:6422
#4  schedule () 		at kernel/sched/core.c:6530
```

会切换至Primary、Secondary的do_idle中。

do_idle会根据是否注册了cpu_idle device，以及device是否可用，来决定进入default idle（wfi），还是调用cpu_device进入idle（psci）

1、default idle (no idle device)

```
    wfi()
#0  cpu_do_idle () 		at arch/arm64/kernel/idle.c:29
#1  arch_cpu_idle () 		at arch/arm64/kernel/idle.c:44
#2  default_idle_call () 	at kernel/sched/idle.c:109
#3  cpuidle_idle_call () 	at kernel/sched/idle.c:191
#4  do_idle () 			at kernel/sched/idle.c:303
```

2、idle device (arm idle device)

```
    wfi() or cpu_ops->suspend()
#0  arm_enter_idle_state	at drivers/cpuidle/cpuidle-arm.c
#1  cpuidle_enter_state		at drivers/cpuidle/cpuidle.c
#2  cpuidle_enter ()		at drivers/cpuidle/cpuidle.c
#3  call_cpuidle ()		at kernel/sched/idle.c
#4  cpu_do_idle () 		at arch/arm64/kernel/idle.c:29
#5  arch_cpu_idle () 		at arch/arm64/kernel/idle.c:44
#6  default_idle_call () 	at kernel/sched/idle.c:109
#7  cpuidle_idle_call () 	at kernel/sched/idle.c:191
#8  do_idle () 			at kernel/sched/idle.c:303
```

## Sysfs




## Files

```
- /kernel/sched/idle.c			# cpu idle sched class (entry)
- /drivers/cpuidle/cpuidle.c		# cpuidle device and driver registation
- /drivers/cpuidle/cpuidle-arm.c	# arm cpu idle driver
- /drivers/cpuidle/dt_idle_states.c	# idle state dts parsing
- /arch/arm64/kernel/idle.c		# arm wfi idle
- /arch/arm64/kernel/cpuidle.c		# arm low level idle (psci suspend)
- /drivers/cpuidle/governor.c		#
- /drivers/cpuidle/sysfs.c		# sysfs interface

```

## Reference

https://zhuanlan.zhihu.com/p/539722367