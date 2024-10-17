# irqchip

irqchip实现一个interrupt controller。

以GIC为例，gic实现为一个irqchip，同时也是一个irqdomain，irqdomain负责virq与hwirq之间的mapping，irqchip负责gic的最终对每个中断的寄存器操作控制。

irqchip也是面向对象思想设计的，可以将一个irqchip的实现（如gic irqchip）当做一个类理解，irqchip提供了类方法，来通过读写gic寄存器，操作每个irq。

irqchip与irqdomain被嵌入到了irqdesc中，最终嵌入到整个中断处理框架，当中断发生时，通过hwirq找到virq，获得irqdesc，再根据irqtype按照通用的中断处理流程，最终调用到平台相关的irqchip接口实现中，完成对中断的响应或控制。

## Files

```
- /include/linux/irq.h		# define irqchip struct (the interface)
- /kernel/irq/chip.c		# irqchip functions
- /include/linux/irqchip.h	# define IRQCHIP_DECLARE
- /drivers/irqchip/irqchip.c	# help to define IRQCHIP_DECLARE
```

## Interface

`IRQCHIP_DECLARE`

声明一个interrupt controller platform device，同时绑定初始化函数。这些irqchip的声明会写入vmlinux的irqchip section中，在初始化时遍历并匹配device tree，进入绑定的初始化函数，执行interrupt controller driver的初始化流程。

`struct irq_chip`

一个interrupt controller driver，需要实现出irq_chip，并绑定到每个irqdesc中，供中断系统使用。这些接口是中断系统定义的其要使用的抽象函数，实现由不同硬件driver完成。这些接口最终完成了寄存器的访问操作流程。

```
	irq_startup:		start up the interrupt 	(defaults to ->enable if NULL)
	irq_shutdown:		shut down the interrupt (defaults to ->disable if NULL)
	irq_enable:		enable the interrupt 	(defaults to ->unmask if NULL)
	irq_disable:		disable the interrupt
	irq_ack:		start of a new interrupt
	irq_mask:		mask an interrupt source
	irq_mask_ack:		ack and mask an interrupt source
	irq_unmask:		unmask an interrupt source
	irq_eoi:		end of interrupt
	irq_set_affinity:	Set the CPU affinity on SMP machines. If the force
	flags:			定义的特性
```

以GICv3 Driver为例：

```
static struct irq_chip gic_chip = {
	.name			= "GICv3",
	.irq_mask		= gic_mask_irq,
	.irq_unmask		= gic_unmask_irq,
	.irq_eoi		= gic_eoi_irq,
	.irq_set_type		= gic_set_type,
	.irq_set_affinity	= gic_set_affinity,
	.irq_retrigger          = gic_retrigger,
	.irq_get_irqchip_state	= gic_irq_get_irqchip_state,
	.irq_set_irqchip_state	= gic_irq_set_irqchip_state,
	.irq_nmi_setup		= gic_irq_nmi_setup,
	.irq_nmi_teardown	= gic_irq_nmi_teardown,
	.ipi_send_mask		= gic_ipi_send_mask,
```

mask / unmask是中断屏蔽、使能流程，eoi最终操作gic eio寄存器，完成ack过程。

startup / shutdown / enable这些，如果driver没有实现，都会默认指向mask函数。

GIC的irq_chip在 `gic_irq_domain_map` 对irq做map的时候，通过 `irq_domain_set_info` 将irq需要的所有数据：irq, hwirq, chip, domain, handle_irq回调绑定到irq对应的desc中。