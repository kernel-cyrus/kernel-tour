# irqwork

A framework to enqueueing and running callbacks in hardirq context.

文件：`/include/linux/irq_work.h`，`/kernel/irq_work.c`

通过IPI发送中断，在中断上下文执行callback handler。

基于smp.c实现（IPI机制，ipi_msg_type: IPI_IRQ_WORK）