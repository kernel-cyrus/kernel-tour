# reboot & poweroff


## Files

```
- /include/linux/reboot.h
- /kernel/reboot.c
- /kernel/power/poweroff.c
- /drivers/firmware/psci/psci.c
```

## Functions

```
r_fmt
sys_off_handler
__weak
emergency_restart
kernel_restart_prepare

register_reboot_notifier
unregister_reboot_notifier

devm_unregister_reboot_notifier
devm_register_reboot_notifier

register_restart_handler
unregister_restart_handler

do_kernel_restart
migrate_to_reboot_cpu
kernel_restart
kernel_shutdown_prepare
kernel_halt

sys_off_notify
alloc_sys_off_handler
free_sys_off_handler
register_sys_off_handler
unregister_sys_off_handler
devm_unregister_sys_off_handler
devm_register_sys_off_handler
devm_register_power_off_handler
devm_register_restart_handler
platform_power_off_notify
register_platform_power_off
unregister_platform_power_off
legacy_pm_power_off
do_kernel_power_off_prepare
do_kernel_power_off
kernel_can_power_off
kernel_power_off
deferred_cad
ctrl_alt_del
run_cmd
__orderly_reboot
__orderly_poweroff
poweroff_work_func
orderly_poweroff
reboot_work_func
orderly_reboot
hw_failure_emergency_poweroff_func
hw_failure_emergency_poweroff
hw_protection_shutdown
reboot_setup
mode_show
mode_store
force_show
force_store
type_show
type_store
cpu_show
cpu_store
kernel_reboot_sysctls_init
kernel_reboot_sysctls_init
reboot_ksysfs_init
```

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