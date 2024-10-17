# syscall

Define:

```
SYSCALL_DEFINE(clone) => __do_sys_clone
```

Stack:

```
#0  kernel_clone () 			at kernel/fork.c:2615
#1  __do_sys_clone () 			at kernel/fork.c:2789
#2  __se_sys_clone () 			at kernel/fork.c:2757
#3  __arm64_sys_clone () 		at kernel/fork.c:2757
#4  __invoke_syscall (syscall_fn=) 	at arch/arm64/kernel/syscall.c:38
#5  invoke_syscall () 			at arch/arm64/kernel/syscall.c:52
#6  el0_svc_common () 			at arch/arm64/kernel/syscall.c:142
#7  do_el0_svc () 			at arch/arm64/kernel/syscall.c:206
#8  el0_svc () 				at arch/arm64/kernel/entry-common.c:624
#9  el0t_64_sync_handler () 		at arch/arm64/kernel/entry-common.c:642
#10 el0t_64_sync () 			at arch/arm64/kernel/entry.S:581
```
