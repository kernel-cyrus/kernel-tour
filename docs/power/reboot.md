# reboot & poweroff

reboot框架提供了以下几种reboot方式：

```
 * RESTART     Restart system using default command and mode.
 * HALT        Stop OS and give system control to ROM monitor, if any.
 * CAD_ON      Ctrl-Alt-Del sequence causes RESTART command.
 * CAD_OFF     Ctrl-Alt-Del sequence sends SIGINT to init task.
 * POWER_OFF   Stop OS and remove all power from system, if possible.
 * RESTART2    Restart system using given command string.
 * SW_SUSPEND  Suspend system using software suspend if compiled in.
 * KEXEC       Restart system using a previously loaded Linux kernel
```

其中，poweroff也作为一种reboot方式实现，可以认为poweroff是一种特殊的reboot。

有以下几条通路触发reboot：

1、reboot syscall

`reboot.c` 中定义了reboot syscall

调用reboot syscall，并传入cmd参数，cmd在uapi/reboot.h中定义，会进入对应类型的reboot流程中。

2、ctrl-alt-del

`ctrl_alt_del()` 在keyboard driver中被调用，与对应key绑定，触发reboot。

3、reboot program

很多rootfs在/sbin下，实现了poweroff、reboot的命令，可以直接调用。

内核也提供了poweroff_cmd的sysctl节点，来写入命令触发reboot

4、watchdog

watchdog thread中，会主动调用kernel_restart来触发reboot。

## Files

```
- /include/linux/reboot.h
- /include/uapi/linux/reboot.h
- /kernel/reboot.c
- /kernel/power/poweroff.c
- /drivers/firmware/psci/psci.c
```

## Functions

`SYSCALL_DEFINE4(reboot`

reboot的syscall中，包含了所有reboot、halt、poweroff的入口。

```
kernel_restart

	kernel_restart_prepare
		call reboot_notifier_list
		device_shutdown, shutdown all devices

	migrate_to_reboot_cpu

	syscore_shutdown
		shutdown all syscore handlers

	machine_restart
		local_irq_disable
		smp_send_stop
		do_kernel_restart
			call restart_handler_list -> psci_sys_reset_nb
```

```
kernel_power_off

	kernel_shutdown_prepare
		call reboot_notifier_list
		device_shutdown, shutdown all devices

	do_kernel_power_off_prepare
		call power_off_prep_handler_list

	migrate_to_reboot_cpu

	syscore_shutdown
		shutdown all syscore handler

	machine_power_off
		local_irq_disable
		smp_send_stop
		do_kernel_power_off
			add pm_power_off -> power_off_handler_list
			call power_off_handler_list -> psci_sys_poweroff
```

上面流程中，有几个主要的callback list：

1、reboot_notifier_list

`register_reboot_notifier`、`unregister_reboot_notifier`

各类机制用于接收reboot的通知，来完成自己的处理。

2、restart_handler_list

`register_restart_handler`、`unregister_restart_handler`

reset驱动用来注册实现reset的流程，比如psci中注册进来的`psci_sys_reset_nb`

3、power_off_handler_list

`register_sys_off_handler`、`unregister_sys_off_handler`

其中，pm_power_off是一个weak函数，可以由底层实现，如psci的`psci_sys_poweroff`

当设置了pm_power_off后，会使用`legacy_pm_power_off`来作为poweroff的流程，并注册到power_off_handler_list，完成调用。

其他几个list：

1、syscore

2、device pm

`emergency_restart`

直接restart，跳过所有destroy流程

## Bootargs

`reboot=`

Set reboot mode and reboot cpu.

## Sysfs

`/sys/kernel/reboot/mode`

Reboot mode. Valid values are: cold warm hard soft gpio

`/sys/kernel/reboot/cpu`

CPU number to use to reboot.

## Sysctrl

`/proc/sys/kernel/poweroff_cmd`

`/proc/sys/kernel/ctrl-alt-del`

## Reference

<https://blog.csdn.net/weixin_41028159/article/details/128108896>