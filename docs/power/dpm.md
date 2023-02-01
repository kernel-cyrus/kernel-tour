# dpm (Device Power Management)

dpm是整个PM Suspend流程的一部分。

每个Device对应了dev_pm_ops，在系统Suspend/Resume时被遍历执行。

DPM对Device的上下电，按顺序提供了以下时刻的回调：

- dpm_prepare

- dpm_suspend

- dpm_suspend_late

- dpm_suspend_noirq

- dpm_resume_noirq

- dpm_resume_early

- dpm_resume

- dpm_resume_complete

这些回调在suspend/resume的过程中顺序执行，每个dpm流程，都会遍历Device List，执行对应的dev_pm_ops操作。

这些操作定义见： 

<https://www.kernel.org/doc/html/v6.1/driver-api/pm/devices.html#entering-system-suspend>

<https://www.kernel.org/doc/html/v6.1/driver-api/pm/devices.html#leaving-system-suspend>

Suspend时，按照bottom-up的顺序，Resume时，按照top-down的顺序。

DPM提供了以下几个链表来管理所有注册进来的Device：

- dpm_list

- dpm_prepared_list

- dpm_suspended_list

- dpm_late_early_list

- dpm_noirq_list

在调用`device_add`创建Device时，`device_pm_add`将device的dev_pm_ops注册到`dpm_list`，之后device在suspend流程中，根据所处状态，在其他几个状态链表中切换。

一个Device，根据其电源Herachy，可以在其所属的以下结构中绑定dev_pm_ops

- dev->pm_domain->ops

- dev->bus->pm

- dev->type->pm

- dev->class->pm

- dev->driver->pm

在Device的Suspend中，会按这个优先级，选择第一个绑定的dev_pm_ops执行。

另外，Device的电源可以在系统正常运行时独立关闭，这部分回调也在dev_pm_ops中实现。这部分在runtime suspend中详细说明。

## Sysfs

`/sys/devices/<device>/power`

Device PM操作和统计节点

## Files

```
- /drivers/base/power/main.c		# Device DPM
```

## Reference

dpm:

<https://www.kernel.org/doc/html/v6.1/driver-api/pm/devices.html>

runtime suspend:

<https://www.kernel.org/doc/html/v6.1/power/runtime_pm.html>

dev_pm_ops:

<https://www.kernel.org/doc/html/v6.1/driver-api/pm/types.html>