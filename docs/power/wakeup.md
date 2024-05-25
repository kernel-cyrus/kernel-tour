# Wakeup

wakeup source（唤醒源），就是在系统下电后（suspend to ram），可以将系统重新唤醒上电的中断。

唤醒机制是ARM提供的，Kernel看，就是一个被配置了wake的irq。

## Files

```
- /drivers/base/power/wakeup.c
```

## Functions

`wakeup_source_create`

创建wakeup source对象

`wakeup_source_register`

创建wakeup source对象，并在/sys/devices/virtual/wakeup创建对应device的节点。

`wakeup_sources_read_lock`

wakeup sources list read lock

`for_each_wakeup_source`

Loopper for all wakeup sources

`device_wakeup_enable`

为device创建对应的wakeup source对象，并绑定wakeup irq。

`device_wakeup_attach_irq`

为device的wakeup source设置wakeup irq

`device_wakeup_arm_wake_irqs`

将所有wakeup source的irq设为wake irq（使能这些irq的唤醒功能）

这个函数在suspend流程中被执行，suspend时将所有wakeup source对应的中断置为wake状态

`device_wakeup_disarm_wake_irqs`

将所有wakeup source的irq的wake disable（禁用这些irq的唤醒功能）

这个函数在resume流程中被执行，resume时将所有wakeup source对应的中断置为diswake状态

`device_init_wakeup`

为device创建对应的wakeup source对象，并绑定wakeup irq。

`pm_stay_awake`

有中断来时，在中断处理函数中，上报wakeup event，并阻止suspend

`pm_relax`

中断处理结束后，释放wakeup event，重新允许suspend

`dev_pm_set_wake_irq`

为device设置wakeup irq

`wakeup_source_activate`

将一个wakeup source标记为active状态，表示有event正在被处理中

`wakeup_source_deactivate`

将一个wakeup source标记为deactive状态，表示event已经处理完毕

## Flows

对于一个device，如果其可以作为唤醒源，可以在dts中配置wakeup-source

在driver中，解析wakeup-source，如果有，则通过`dev_pm_set_wake_irq`配置device的wakeup irq

然后调用wakup框架相关的device_init_wakeup，为device创建出wakeup source，并绑定wakeup irq

系统suspend时，会通过device_wakeup_arm_wake_irqs为device的wakeup irq配置wake

当中断发生是，系统被唤醒，resume流程被执行

在中断回调函数中，driver使用`pm_stay_awake`上报event，在处理结束后，通过`pm_relax`结束处理。

同时，pm_stay_awake被用于wakelock，当wakelock被设置时，也会作为一个wakeup event上报，将suspend流程禁用。

## sysfs

所有唤醒源列表（/drivers/base/power/wakeup.c）

```
root@test:/ # cat /sys/kernel/debug/wakeup_sources                    
name		active_count	event_count	wakeup_count	expire_count	active_since	total_time	max_time	last_change	prevent_suspend_time
event1      	40644		40644		0		0		0		31294		30		537054822		0
event4      	4496		4496		0		0		0		13369		22		20913677		0
event5      	4496		4496		0		0		0		13048		22		20913677		0
event0      	4540		4540		0		0		0		27995		277		258270184		0
eventpoll   	40688		54176		0		0		0		217		5		537054822		0
NETLINK     	2175		2175		0		0		0		16960		59		537058523		0
```

```
root@AGX-Orin:/sys/class/wakeup# ls -l
total 0
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup1 -> ../../devices/platform/c2a0000.rtc/wakeup/wakeup1
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup10 -> ../../devices/platform/3400000.sdhci/wakeup/wakeup10
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup11 -> ../../devices/platform/3610000.xhci/usb1/1-4/1-4.4/wakeup/wakeup11
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup12 -> ../../devices/platform/gpio-keys/wakeup/wakeup12
lrwxrwxrwx 1 root root 0 May 25 14:04 wakeup13 -> ../../devices/platform/c240000.i2c/i2c-1/1-0008/wakeup/wakeup13
lrwxrwxrwx 1 root root 0 May 25 14:04 wakeup14 -> ../../devices/platform/c240000.i2c/i2c-1/1-0008/power_supply/ucsi-source-psy-1-00081/wakeup14
lrwxrwxrwx 1 root root 0 May 25 15:17 wakeup15 -> ../../devices/platform/c240000.i2c/i2c-1/1-0008/power_supply/ucsi-source-psy-1-00082/wakeup15
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup2 -> ../../devices/platform/c2a0000.rtc/rtc/rtc1/alarmtimer.0.auto/wakeup/wakeup2
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup3 -> ../../devices/platform/bpmp/bpmp:i2c/i2c-4/4-003c/nvvrs-pseq-rtc/wakeup/wakeup3
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup4 -> ../../devices/platform/efuse-burn/wakeup/wakeup4
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup5 -> ../../devices/platform/3610000.xhci/usb1/wakeup/wakeup5
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup6 -> ../../devices/platform/3610000.xhci/usb2/wakeup/wakeup6
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup7 -> ../../devices/platform/3610000.xhci/wakeup/wakeup7
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup8 -> ../../devices/virtual/wakeup/wakeup8
lrwxrwxrwx 1 root root 0 Mar 15  2023 wakeup9 -> ../../devices/virtual/wakeup/wakeup9
```

所有唤醒源绑定的device列表（/drivers/base/power/wakeup_stats.c）

```
/sys/devices/virtual/wakeup/wakeup<dev_id>
uevent
event_count
max_time_ms
wakeup_count
total_time_ms
expire_count
active_count
subsystem
last_change_ms
prevent_suspend_time_ms
name
active_time_ms
```

控制Device的唤醒源：

在/sys/devices/\<device\>/power、/sys/devices/\<device\>/\<wakeup\>下，会创建wakeup控制节点，可以enable/disable一个device的唤醒源。

当系统被唤醒时，可以通过/sys/power/pm_wakeup_irq查看唤醒源。

## Reference

<http://www.wowotech.net/pm_subsystem/wakeup_events_framework.html>

<http://www.manongjc.com/detail/56-hiawkqvmprdijht.html>
