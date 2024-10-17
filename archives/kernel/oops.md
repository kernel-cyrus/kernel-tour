# oops

Kernel在发生异常时，会打印出oops日志，oops中包括了问题的现场信息。

如果是严重问题，导致Kernel运行不再可信，则会执行panic，将系统终止运行。

在代码中，我们也可以加入BUG检查，在不应该进入的分支加入WARN_ON或BUG_ON，将BUG report出来。

## Files

```
- /kernel/panic.c
- /include/asm-generic/bug.h
```

## Format

```
Unable to handle kernel NULL pointer dereference at virtual address 00000000
pgd = eeda0000
[00000000] *pgd=aedb8831, *pte=00000000, *ppte=00000000
Internal error: Oops: 817 [#1] PREEMPT ARM
Modules linked in: musb_am335x(+) rtc_omap omap_wdt ti_am335x_tscadc matrix_keypad matrix_keymap
CPU: 0 PID: 135 Comm: udevd Not tainted 4.4.48-02799-g2f0993afde90-dirty #440
Hardware name: Generic AM33XX (Flattened Device Tree)
task: eeeaa400 ti: eeeda000 task.ti: eeeda000
PC is at am335x_child_probe+0x2c/0x58 [musb_am335x]
LR is at am335x_child_probe+0x24/0x58 [musb_am335x]
pc : [<bf01902c>] lr : [<bf019024>] psr: 600b0013
sp : eeedbcb8 ip : eeedbcb8 fp : eeedbccc
r10: 00000000 r9 : 0000000e r8 : bf019230
r7 : fffffdfb r6 : bf019230 r5 : ee99aa00 r4 : ee99aa10
r3 : ee99aa10 r2 : 00000000 r1 : 00000001 r0 : ee99aa10
Flags: nZCv IRQs on FIQs on Mode SVC_32 ISA ARM Segment none
Control: 10c5387d Table: aeda0019 DAC: 00000051
Process udevd (pid: 135, stack limit = 0xeeeda210)
Stack: (0xeeedbcb8 to 0xeeedc000)
bca0: ee99aa10 ee99aa10
bcc0: eeedbcec eeedbcd0 c03772b4 bf01900c ee99aa10 c089f530 00000000 c08686b0
...
Backtrace:
[<bf019000>] (am335x_child_probe [musb_am335x]) from [<c03772b4>] (platform_drv_probe+0x5c/0xc0)
[<c0377258>] (platform_drv_probe) from [<c037501c>] (driver_probe_device+0x228/0x484)
[<c0374df4>] (driver_probe_device) from [<c0375314>] (__driver_attach+0x9c/0xa0)
[<c0375278>] (__driver_attach) from [<c0372dbc>] (bus_for_each_dev+0x7c/0xb0)
[<c0372d40>] (bus_for_each_dev) from [<c0374934>] (driver_attach+0x28/0x30)
```

## Functions

`panic`

执行panic，打印现场oops，并终止系统运行。内核在很多Exception Handler中，执行了panic()。

`BUG_ON`

判断bug条件，触发panic。

`WARN_ON`

判断bug条件，只report oops，不触发panic。

## Usage Test

### Enalbe more debug info on panic

向cmdline添加panic_print=0xff，使能panic更多打印，参考值在 `panic.c` 中定义。

### Trigger panic in WARN_ON

向cmdline添加panic_on_warn=1，在WARN_ON的地方触发panic

### Stop on first oops

向cmdline添加pause_on_oops=1，会在第一个panic处停止，防止oops刷屏。

## Module Test

[test-oops](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-oops)

## Debug Kernel Oops

[Linux Kernel Debugging](https://training.ti.com/debugging-embedded-linux-kernel-oops-logs)

[Debugging Embedded Linux Systems: Understand Kernel Oops Logs](https://d3s.mff.cuni.cz/files/teaching/nswi161/vlastimil-babka-kernel-debugging.pdf)
