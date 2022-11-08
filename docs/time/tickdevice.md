# tickdevice

tickdevice是基于clockevent device实现的periodic的tick机制。

每个cpu有一个独立的tickdevice，指向了当前核可以使用的精度最高的clockevent device。

通过向这个clockevent device挂入tick handler，并在handler中周期性计算和设置下次触发时间，从而利用clockevent device实现tick的周期性触发。

tick触发时会做几件事，比如运行系统注册的所有timer，更新timekeeping时间，更新jiffies。

调度是利用tick来完成的，可以认为tick提供了timer触发机制，而调度是注册进来的一个timer。

**tick broadcast**

?

## Files

```
- /kernel/time/tick-common.c
```

## Variables

`tick_cpu_device`

PER CPU的tick device

`tick_do_timer_cpu`

CPU0

`tick_next_period`

使用CPU0来对next time时间做tracking，这个变量会在每次CPU0 tick中被更新。这个变量应该是供其他模块获取下次tick时间使用。

## Functions

`tick_check_new_device`

在clockevent device新注册时被调用，使用新注册的clockevent device绑定到当前核的tick device上。

在使用新的clockevent device前，会做一些检查：

- 通过 `tick_check_percpu` 检查clockevent device是否是本核的。
- 通过 `tick_check_preferred` 检查新的clockevent device精度是否高于原来的

然后调用 `tick_setup_device` 将新的clockevent device绑定到当前tick device上。

`tick_setup_device`

绑定clockevent device，并将tick的回调 `tick_handle_periodic` 设置给clockevent device，然后根据clockevent device的mode，来设置周期触发。如果clockevent device只支持ONESHOT（arch timer），则通过 `tick_next_period` 周期性设置下次触发时间。

`tick_handle_periodic`

周期tick handler。timer loop框架，调用 `tick_periodic` 执行操作，并使用clockevent接口，在handle完这个tick后，设置下次触发，从而实现timer loop。

`tick_periodic`

tick的执行内容。在一个tick中，会做几件事：

- 更新jiffies
- 更新平均占用率（loadavg）
- 更新墙上时间（使用clocksource更新timekeeping）
- 运行timer（timer接口挂上来的timer）
- profile tick?

`tick_suspend` / `tick_resume`

?

`tick_freeze` / `tick_unfreeze`

?

## Sysfs

`/sys/devices/clockevents/clockevents<cpu>`

- current_device：当前tickdevice使用的clock eventdevice
- unbound_device：remove

`/sys/devices/clockevents/broadcast`

- current_device：tick broadcast device使用的clock eventdevice

## Reference

<https://www.youtube.com/watch?v=Puv4mW55bF8&ab_channel=TheLinuxFoundation>