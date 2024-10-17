# rpm (Runtime Suspemd)

Device的Suspend一共有两种情况：

- System Sleep：通过操作/sys/power/state，在全系统Suspend流程中，Device进入Suspend。

- Runtime Suspend：在系统正常运行时，对部分可下电的Device进入Runtime Suspend

两种情况都会调用device->dev_pm_ops，System Sleep使用suspend类接口，Runtime Suspend使用runtime_suspend类接口。

Runtime Suspend提供了三种状态：

- runtime_idle：Suspend之前的一种临时状态，表示可以Suspend，并且用于Idle计时

- runtime_suspend：进入Runtime Suspend

- runtime_resume：进入Runtime Resume

Device有两种情况触发进入Runtime Suspend：

- Device的Suspend Timer到期（处于Idle足够长时间）

- Put PM Device到usage_count=0（没人继续使用Device）

使Device进入Suspend，支持同步和异步两种方式，同步方式直接执行，异步方式通queue work执行。

## Files

```
- /drivers/base/power/main.c		# DPM（Device Power Management）
- /drivers/base/power/runtime.c		# RPM（Device Runtime Suspend）
- /linux/pm_runtime.h			# RPM Main Interface
- /drivers/base/power/sysfs.c		# Sysfs
```

## Functions

**同步接口**

- `pm_runtime_idle`：`rpm_idle` -> device->dev_pm_ops->runtime_idle

- `pm_runtime_suspend`：`rpm_suspend` -> device->dev_pm_ops->runtime_suspend
	rpm_suspend
- `pm_runtime_resume`：`rpm_resume` -> device->dev_pm_ops->runtime_resume
	rpm_resume
- `pm_runtime_autosuspend`：`rpm_suspend(RPM_AUTO)`，到期则suspend，没到期再继续等


**同步接口（Force）**

- `pm_runtime_force_suspend`

- `pm_runtime_force_resume`

**异步接口**

- `pm_request_idle`

- `pm_request_resume`

- `pm_request_autosuspend`

向device->power->request写入Request，添加一个Work到WorkQueue（pm_runtime_work），判断执行Request对应流程。

**初始化**

`pm_runtime_init`

在`device_initialize`中初始化RPM

- 初始化异步执行的Work（pm_runtime_work）

- 初始化RPM的到期Timer（pm_suspend_timer_fn）

`pm_schedule_suspend`

启动Device的Runtime Suspend Timer

**PUT、GET**

- `pm_runtime_get`：`__pm_runtime_resume` -> usage_count + 1

- `pm_runtime_put`：`__pm_runtime_suspend` -> usage_count - 1

实际上就是重复调用suspend/resume，只有当usge_count为0时，才会真正触发执行。

**Enable、Disable**

- `pm_runtime_enable`

- `pm_runtime_disable`

**Others**

`rpm_callback`

因为dev的dev_pm_ops可能来源于class、type、bus、device，如果每个接口中都写一轮遍历会很冗余，所以实现了这个接口，通过名字自动选择可用的callback

## Sysfs

`/sys/devices/<device>/power/control`

Enable / Disable Device Runtime Suspend

`/sys/devices/<device>/power/runtime_active_time`

设备Active时间

`/sys/devices/<device>/power/runtime_suspend_time`

设备Runtime Suspend时间

`/sys/devices/<device>/power/runtime_status`

设备Runtime Suspend状态

`/sys/devices/<device>/power/autosuspend_delay_ms`

设备自动Runtime Suspend时间（Idle这么久进入Runtime Suspend）

## Reference

<http://www.wowotech.net/pm_subsystem/rpm_overview.html>

<https://www.kernel.org/doc/html/v6.1/driver-api/pm/devices.html#runtime-power-management>

<https://www.kernel.org/doc/html/v6.1/power/runtime_pm.html>