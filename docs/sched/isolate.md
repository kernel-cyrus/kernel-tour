# cpu isolation

初始化

nohz_full=
	housekeeping_nohz_full_setup
		housekeeping_setup
			tick_nohz_full_setup(cpumask)

isolcpus=
	housekeeping_isolcpus_setup
		housekeeping_setup

housekeeping_setup
	housekeeping_mask -> 允许调度的核
	non_housekeeping_mask -> 隔离核（isolcpus）
	housekeeping -> 各种isol功能对应的cpumask

start_kernel			(main.c)
	housekeeping_init	(isolation.c)


HK_TYPE_TICK
sched_tick_start bypass
sched_tick_stop bypass

HK_TYPE_SCHED
nohz_balance_enter_idle
nohz_newidle_balance

HK_TYPE_WQ
workqueue_init_early(wq_unbound_cpumask)

HK_TYPE_DOMAIN
sched_init_domains

Reference:

https://lwn.net/Articles/816298/

https://lwn.net/Articles/270623/

https://www.suse.com/c/cpu-isolation-full-dynticks-part2/