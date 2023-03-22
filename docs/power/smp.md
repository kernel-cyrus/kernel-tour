# smp

本文主要记录多核启动流程，包括主核（Primary CPU）和从核（Secondary CPU），以及IPI通信支持。

## Primary CPU Boot Flow

```
-> primary_entry	(arm64/head.S)

	get fdt addr
	init el
	create init pagetable
	-> __primary_switch
		enable mmu
	-> __primary_switched

	(ttbr1_el1: idmap)

-> start_kernel		(/init/main.c)

	-> setup_arch
		setup fdt
		setup command line
		parse early param
		init erly console
		-> paging_init
			map kernel segment
			switch to final pagetable
			free id map table

		(ttbr1_el1: pg_swapper_dir)

	-> rest_init
		create kernel_init thread (pid=1)

	-> do_idle()

-> kernel_init		(/init/main.c)

	-> kernel_init_freeable
		-> smp_init
			cpu_up
			wait for up complete
		-> sched_init_smp
			add secondary cpus to sched
		-> do_initcalls
			init all modules
			...
			populate_rootfs
			...
		-> wait_for_initramfs

	<- ret_from_fork

	->run_init_process => init
```

## Secondary Boot Flow

```
secondary_entry		(arm64/head.S)

	secondary_startup
	__cpu_setup
	__enable_mmu (ttbr1_el1: swapper_pg_dir)
	secondary_switched

secondary_start_kernel	(arm64/smp.c)

	init cpu_ops
	-> notify_cpu_starting
		call cpuhp callbacks
			enable gic
			enable timer

	-> complete(&cpu_running)
		notify primary cpu to continue

	-> cpu_startup_entry
		idle()
```

## IPI Support (inter-processor call)

smp实现了IPI支持（CPU内部核间通信），定义了以下IPI Message类型：

```
enum ipi_msg_type {
	IPI_RESCHEDULE,		# 0号中断, 重新调度进程,scheduler_ipi()
	IPI_CALL_FUNC,		# 1号中断, 调用generic_smp_call_function_interrupt()（smp_call）
	IPI_CPU_STOP,		# 2号中断，调用local_cpu_stop(), 使处理器停止
	IPI_CPU_CRASH_STOP,	# 3号中断，调用ipi_cpu_crash_stop(),使处理器停止
	IPI_TIMER,		# 4号中断，调用tick_receive_broadcast(),广播时钟事件
	IPI_IRQ_WORK,		# 5号中断，调用irq_work_run()（irq_work.h，允许在硬中断中执行回调）
	IPI_WAKEUP,		# 6号中断，调用acpi_parking_protocol_valid(cpu), 唤醒处理器
	NR_IPI
};
```

IPI的每个类型都实现了一种通信机制，或者说每个IPI通讯机制都定义了自己独立的msg type。

IPI中断通过`smp_cross_call`发送，通过`do_handle_IPI`Handle。

IPI是linux定义的虚拟中断类型，在ARM上以SGI实现。

`smp_cross_call`

发送（raise）一个ipi_msg_type类型的IPI中断给某个CPU（cpu_mask）

`smp_call_function`

Run a function on all other CPUs

`smp_call_function_any`

Run a function on any of the given cpus (sync)

`smp_call_function_single_async`

Run a function on a single cpu (async)

Reference: <https://blog.csdn.net/weixin_42135087/article/details/123191551>

## smp functions

`smp_init`

bringup secondary cpus

`call_function_init`

enable call function

`smp_processor_id`

get current process id

`on_each_cpu`

loop for each online cpu

`on_each_cpu_mask`

loop for each cpu in mask

`get_cpu`、`put_cpu`

获取当前CPU ID并关闭抢占，put打开抢占

## bootargs

`maxcpus`

启动时，bringup N个CPU。

启动后，可以手动启动其他CPU：`echo 1 > /sys/devices/system/cpu/cpuX/online`

`nr_cpus`

设置最大支持的cpu，nr_cpus=1，单核模式

`nosmp`

Disable SMP，等于maxcpus=0

## Files

```
- /kernel/smp.c			# Kernel SMP Init, SMP Call
- /include/linux/smp.h		# Kernel SMP Interfaces
- /arch/arm64/kernel/smp.c	# ARM secondary_start_kernel
```