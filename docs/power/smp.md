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