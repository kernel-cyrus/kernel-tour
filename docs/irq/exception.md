# exception, fault, traps

ARM的每个EL有四类Exception：

- Sync（WFE/WFI，SVC/HVC/SMC，Data Abort，Undef Instruction...）
- IRQ（PPI、SPI、SGI）
- FIQ（Secure、Non-Secure Transfer）
- SERROR（Hardware Error，Bus Error）

entry.S中定义了exception vector table，el1、el2的exception，根据异常类型，会路由到对应的handler中去处理。

所有handler在exception.h中声明，在entry-common.c中实现。

对于sync，会根据esr判断具体异常类型，进入到fault或trap中进行处理。

对于irq、fiq，会通过gic driver挂上来的gic_handle_irq、gic_handle_fiq，从iar获得中断号，最终由irq domain路由到最终的中断处理函数。

对于serror，会直接进入panic。

**fault vs trap**

sync类型异常主要包括以下几类：（见traps.c:esr_class_str）

- WFI/WFE
- SVC、HVC、SMC
- PAC（PTRAUTH FAIL）
- Debug：Breakpoint、Step、Watchpoint
- Unknown Instruction
- PC Align
- Data Abort（内存访问异常）
- ...

其中Abort为各种内存异常访问，又可以根据esr中的几个bit继续细分：（见fault.c:fault_info）

- Page Fault
- Alignment Fault
- Translation Fault
- ...

所有Data Abort，统称为fault，在fault.c中处理。

其他Sync类型Exception，统称为traps，在traps.c中处理。

各个EL层面发生Sync Exception后的处理方式并不相同，大体上，EL0发生的Sync，会最终转为Signal发送给用户进程，如果用户进程没有绑定Handle函数，则会导致进程异常退出。如果是EL1内核层发生的Sync，大概率会直接转为Panic。这个在Sync Handler中，会判断当前是否为user space。

除此之外，traps机制还为各种traps类型提供了hook，允许其他机制监控和处理各类异常，或在异常发生时打印出额外信息。

## exception handlers

**entry.S**

```
/*
 * Exception vectors.
 */
	kernel_ventry	1, sync				// Synchronous EL1h
	kernel_ventry	1, irq				// IRQ EL1h
	kernel_ventry	1, fiq_invalid			// FIQ EL1h
	kernel_ventry	1, error			// Error EL1h
  ...
```

其中，1代表el1，sync是entry.S中内部定义的函数名，通过kernel_ventry最终转换出来的函数名为el1_sync。

```
SYM_CODE_START_LOCAL_NOALIGN(el1_sync)
	kernel_entry 1
	mov	x0, sp
	bl	el1_sync_handler
	kernel_exit 1
SYM_CODE_END(el1_sync)
```

可以看到，el1_sync中，最终跳入el1_sync_handler

完整的handler table如下：

```
el0 sync:    el0_sync_handler     (entry-common.c)
el0 irq:     gic_handle_irq       (irq-gic-v3.c)
el0 fiq:     bad_mode (INVALID)   (traps.c)
el0 error:   do_serror            (traps.c)
el1 sync:    el1_sync_handler     (entry-common.c)
el1 irq:     gic_handle_irq       (irq-gic-v3.c)
el1 fiq:     bad_mode (INVALID)   (traps.c)
el1 error:   do_serror            (traps.c)
```

## Files

```
- /arch/arm64/kernel/entry.S		# Exception Vector Table
- /arch/arm64/kernel/entry-common.c	# Exception Handler Implementation
- /arch/arm64/include/asm/exception.h	# Exception Handler Declaration
- /arch/arm64/mm/fault.c		# Abort handling
- /arch/arm64/kernel/traps.c		# Trap handling
- /kernel/irq/handle.c			
```
