# suspend / resume


Linux内核提供了4种Suspend模式:

```
freeze	（suspend to idle）	进程冻结，CPU进入Idle，外设进入Suspend（s2idle）
standby	（suspend to standby）	从核拔除下电，只保留主核，系统正产运行（不使用）
mem	（suspend to mem）	CPU、外设均下电，进程冻结，DDR进入自刷新（主要流程）
disk	（suspend to disk）	CPU、DDR、外设均下电，所有数据进入Disk（hibernate，不使用）
```

通过用户空间向”/sys/power/state”写入”freeze”、”standby”、”mem”来触发对应Suspend流程。

Android Suspend也在用户态通过写节点来触发。

Suspend主要包括5件事：

1、冻结进程

需要将所有进程置为Freeze状态，保存现场上下文，暂停运行。在Resume后重新使其继续运行。

2、设备休眠（DPM）

Device休眠通过dpm框架来管理，DPM是一个PM设备链表，Device电源管理框架在DPM中详细描述。

3、系统休眠（Platform）

suspend_ops在psci初始化时，通过suspend_set_ops注册。在Platform Suspend阶段，通过PSCI下发给ATF完成CPU及整个系统的Suspend动作。

4、Syscore Suspend

syscore允许subsys向suspend过程挂回调，比实现一个pm device，这个量级更轻。

<https://lore.kernel.org/lkml/201103100133.06734.rjw@sisk.pl/>

5、PM Notifiers

还有一些回调点，需要在suspend前，或resume后完成一些事。这时可以使用PM Notifier，通过register_pm_notifier注册回调。

通过ftrace event可以清楚追踪到这些流程。

## Files

```
- /kernel/power/suspend.c	# main suspend flow
- /kernel/power/main.c		# /sys/power interface
- /drivers/base/power/main.c	# dpm (device power management)
- /drivers/base/syscore.c	# pm callback framework
```

## Functions

`pm_suspend/enter_state`

```
suspend_prepare

	suspend_freeze_processes

		pm_notifier_call_chain_robust	# Notifiers: PM_SUSPEND_PREPARE

		suspend_freeze_processes	# 冻结进程

		pm_notifier_call_chain 		# Notifiers: PM_POST_SUSPEND

suspend_devices_and_enter

	platform_suspend_begin			# Platfom:suspend_ops->begin

	suspend_console				# Disable Printk and Console

	dpm_suspend_start			# DPM: Suspend all Device

	suspend_enter

		platform_suspend_prepare	# Platfom: suspend_ops->prepare

		dpm_suspend_late		# DPM: Suspend all Device (late)
	
		platform_suspend_prepare_late

		dpm_suspend_noirq		# DPM: Suspend all Device (noirq)

		platform_suspend_prepare_noirq	# Platfom: suspend_ops->prepare_late

		pm_sleep_disable_secondary_cpus

		arch_suspend_disable_irqs	# 禁中断

		syscore_suspend			# Syscore: Suspend all syscore callbacks

		suspend_ops->enter(state)	# Platfom: enter state
```

## Sysfs

Tracing suspend/resume

\* 操作需要在USB断开的情况下进行

```
cd /sys/kernel/debug/tracing
echo 1 > events/power/suspend_resume/enable
echo 1 > tracing_on
```

```
<...>-4745  [001] .... 280208.503406: suspend_resume: suspend_enter[3] begin
<...>-4745  [001] .... 280208.503416: suspend_resume: sync_filesystems[0] begin
<...>-4745  [000] .... 280208.566895: suspend_resume: sync_filesystems[0] end
<...>-4745  [000] .... 280208.577584: suspend_resume: freeze_processes[0] begin
<...>-4745  [000] .... 280208.597040: suspend_resume: freeze_processes[0] end
<...>-4745  [000] .... 280208.597048: suspend_resume: suspend_enter[3] end
<...>-4745  [000] .... 280208.597053: suspend_resume: dpm_prepare[2] begin
<...>-4745  [000] .... 280208.600119: suspend_resume: dpm_prepare[2] end
<...>-4745  [000] .... 280208.600124: suspend_resume: dpm_suspend[2] begin
<...>-4745  [000] .... 280208.600354: lpi_pinctrl_suspend: lpi_pinctrl_suspend: system suspend
<...>-4745  [000] .... 280208.641639: suspend_resume: dpm_suspend[2] end
<...>-4745  [000] .... 280208.641654: suspend_resume: dpm_suspend_late[2] begin
<...>-4745  [000] .... 280208.647185: suspend_resume: dpm_suspend_late[2] end
<...>-4745  [000] .... 280208.648907: suspend_resume: dpm_suspend_noirq[2] begin
<...>-4745  [000] .... 280208.653170: suspend_resume: dpm_suspend_noirq[2] end
<...>-4745  [000] .... 280208.653211: suspend_resume: CPU_OFF[1] begin
<...>-4745  [000] .... 280208.657886: suspend_resume: CPU_OFF[1] end
<...>-4745  [000] .... 280208.657890: suspend_resume: CPU_OFF[2] begin
<...>-4745  [000] .... 280208.661211: suspend_resume: CPU_OFF[2] end
<...>-4745  [000] .... 280208.661214: suspend_resume: CPU_OFF[3] begin
<...>-4745  [000] .... 280208.663980: suspend_resume: CPU_OFF[3] end
<...>-4745  [000] .... 280208.663983: suspend_resume: CPU_OFF[4] begin
<...>-4745  [000] .... 280208.666587: suspend_resume: CPU_OFF[4] end
<...>-4745  [000] .... 280208.666589: suspend_resume: CPU_OFF[5] begin
<...>-4745  [000] .... 280208.670508: suspend_resume: CPU_OFF[5] end
<...>-4745  [000] .... 280208.670511: suspend_resume: CPU_OFF[6] begin
<...>-4745  [000] .... 280208.674076: suspend_resume: CPU_OFF[6] end
<...>-4745  [000] .... 280208.674079: suspend_resume: CPU_OFF[7] begin
<...>-4745  [000] .... 280208.677074: suspend_resume: CPU_OFF[7] end
<...>-4745  [000] d... 280208.677145: suspend_resume: syscore_suspend[0] begin
<...>-4745  [000] d... 280208.677163: suspend_resume: syscore_suspend[0] end
<...>-4745  [000] d... 280208.677163: suspend_resume: machine_suspend[3] begin
<...>-4745  [000] d... 280208.677163: suspend_resume: machine_suspend[3] end
<...>-4745  [000] d... 280208.677163: suspend_resume: syscore_resume[0] begin
<...>-4745  [000] dn.. 280208.677212: suspend_resume: syscore_resume[0] end
<...>-4745  [000] .... 280208.677703: suspend_resume: CPU_ON[1] begin
<...>-4745  [000] .... 280208.680579: suspend_resume: CPU_ON[1] end
<...>-4745  [000] .... 280208.680957: suspend_resume: CPU_ON[2] begin
<...>-4745  [000] .... 280208.682403: suspend_resume: CPU_ON[2] end
<...>-4745  [000] .... 280208.683170: suspend_resume: CPU_ON[3] begin
<...>-4745  [000] .... 280208.684575: suspend_resume: CPU_ON[3] end
<...>-4745  [000] .... 280208.684727: suspend_resume: CPU_ON[4] begin
<...>-4745  [000] .... 280208.686221: suspend_resume: CPU_ON[4] end
<...>-4745  [000] .... 280208.686371: suspend_resume: CPU_ON[5] begin
<...>-4745  [000] .... 280208.687832: suspend_resume: CPU_ON[5] end
<...>-4745  [000] .... 280208.687978: suspend_resume: CPU_ON[6] begin
<...>-4745  [000] .... 280208.689916: suspend_resume: CPU_ON[6] end
<...>-4745  [000] .... 280208.690110: suspend_resume: CPU_ON[7] begin
<...>-4745  [000] .... 280208.691842: suspend_resume: CPU_ON[7] end
<...>-4745  [000] .... 280208.692035: suspend_resume: dpm_resume_noirq[16] begin
<...>-4745  [006] .... 280208.696154: suspend_resume: dpm_resume_noirq[16] end
<...>-4745  [007] .... 280208.697518: suspend_resume: dpm_resume_early[16] begin
<...>-4745  [005] .... 280208.702554: suspend_resume: dpm_resume_early[16] end
<...>-4745  [005] .... 280208.702563: suspend_resume: dpm_resume[16] begin
<...>-4745  [002] .... 280208.721405: suspend_resume: dpm_resume[16] end
<...>-4745  [002] .... 280208.721412: suspend_resume: dpm_complete[16] begin
<...>-4745  [002] .n.. 280208.724622: suspend_resume: dpm_complete[16] end
<...>-4745  [002] .... 280208.724784: suspend_resume: resume_console[3] begin
<...>-4745  [002] .... 280208.724786: suspend_resume: resume_console[3] end
<...>-4745  [002] .... 280208.724790: suspend_resume: thaw_processes[0] begin
<...>-4745  [002] .... 280208.734586: suspend_resume: thaw_processes[0] end
```

可以看到suspend/resume的整个框架流程。

查看唤醒源：

```
cat /sys/power/pm_wakeup_irq	# 唤醒源中断号
cat /proc/interrupt		# 查看中断对应设备名
```

## Reference

<https://zhuanlan.zhihu.com/p/542445635>

<https://zhuanlan.zhihu.com/p/568050822>

<https://www.cnblogs.com/LoyenWang/p/11372679.html>