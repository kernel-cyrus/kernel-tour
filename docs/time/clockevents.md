# clockevents

> clock event device is used to abstract a device that can raise interrupt based on time. (hardware timer device)

clockevent封装了hardware timer driver，并提供了设置下次event和event回调函数的功能。

上层机制，通过clockevent device来访问hardware timer功能，并通过封装自己的event handler和set_next_event，从而实现出自己的功能机制（tick device）

**hardware timer**

硬件timer就是两个寄存器，一个自增的tval(time value)，和一个可设置的cval(comporator value)，当自增到cval时，则触发一次timer中断。

Timer可被设置为多种模式（oneshot、periodic、ktime）。oneshot触发一次中断则停止，periodic会按照固定周期连续触发，ktime可以设置具体触发时间点。（Arm arch timer只有oneshot模式）

Arm的每个核，有四个Timer，这四个Timer由system counter驱动。这四个timer会创建出percpu的clockevent device。

## Files

```
- /include/linux/clockchips.h
- /mainline/kernel/time/clockevents.c
```

## Structures

`clock_event_device`

clock event device，包括了私有变量核类函数（回调），功能如上介绍。

clockevent device数据结构及功能包括：clockevent device的名字，精度，timer中断号，event处理函数（非中断处理函数），中断绑核，timer模式switch函数，timer的suspend/resume函数，timer支持的features(oneshot, periodic)，设置下一个timer event时间

clockevent的event handler，并非是hardware timer irq的handler，irq handler在arch timer的驱动中直接注册了自己的handler，这个event handler是在irq handler中被调用的。(在tickdevice中设置？)

clockevent的freq，就是在oneshot的硬件feature下，实现perioidic功能。与clocksource计算freq的方法类似，二者都使用mult/shift来表示freq，以避免使用除法。

## Variables

`clockevent_devices`

全局clockevent device链表

## Functions

`clockevents_config`

设置clockevent device的触发频率

`clockevents_register_device`

将clockevent device注册到clockevent devices链表上。

`clockevents_config_and_register`

上面两个函数的叠加

`clockevents_program_event`

设置下次中断触发时间（event时间）

`clockevents_switch_state`

设置clockevent device的模式

`clockevents_suspend / resume`

clockevent device suspend / resume

## Reference

<https://www.kernel.org/doc/Documentation/timers/timekeeping.txt>