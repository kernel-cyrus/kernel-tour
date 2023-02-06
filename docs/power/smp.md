# smp

本文主要记录多核启动流程，包括主核（Primary CPU）和从核（Secondary CPU）。

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

##

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

`smp_call_function`

Run a function on all other CPUs

`smp_call_function_any`

Run a function on any of the given cpus (sync)

`smp_call_function_single_async`

Run a function on a single cpu (async)

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