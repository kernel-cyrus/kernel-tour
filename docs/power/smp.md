# smp

/arch/arm64/kernel/smp.c

primary:

```
#0  do_idle () at kernel/sched/idle.c:259
#1  0xffff8000080d0e64 in cpu_startup_entry (state=state@entry=CPUHP_ONLINE) at kernel/sched/idle.c:400
#2  0xffff800008f49cc4 in rest_init () at init/main.c:726
#3  0xffff800009860b60 in arch_call_rest_init () at init/main.c:882
#4  0xffff800009861208 in start_kernel () at init/main.c:1137
#5  0xffff8000098603ec in __primary_switched () at arch/arm64/kernel/head.S:468
```

secondary:

```
#0  do_idle () at kernel/sched/idle.c:259
#1  0xffff8000080d0e64 in cpu_startup_entry (state=state@entry=CPUHP_AP_ONLINE_IDLE) at kernel/sched/idle.c:400
#2  0xffff800008025830 in secondary_start_kernel () at arch/arm64/kernel/smp.c:265
#3  0xffff800008f643b4 in __secondary_switched () at arch/arm64/kernel/head.S:661
```

idle:

```
#0  cpu_do_idle () at arch/arm64/kernel/idle.c:32
#1  0xffff800008f497f0 in arch_cpu_idle () at arch/arm64/kernel/idle.c:44
#2  0xffff800008f52e38 in default_idle_call () at kernel/sched/idle.c:109
#3  0xffff8000080d0c0c in cpuidle_idle_call () at kernel/sched/idle.c:191
#4  do_idle () at kernel/sched/idle.c:303
#5  0xffff8000080d0e68 in cpu_startup_entry (state=state@entry=CPUHP_ONLINE) at kernel/sched/idle.c:400
#6  0xffff800008f49cc4 in rest_init () at init/main.c:726
#7  0xffff800009860b60 in arch_call_rest_init () at init/main.c:882
```