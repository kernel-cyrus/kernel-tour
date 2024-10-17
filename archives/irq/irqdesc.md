# irqdesc

Linux为每个中断分配了一个全局统一唯一的irq number(virq)，这个irq号不是hwirq，virq与hwirq的mapping是通过irqdomain来完成的。

irqdesc中断描述符保存了每个irq的数据，包括每个irqdesc中，保存了hwirq号，irq的类型（level, edge）及该类型irq的通用处理函数（handle_irq），注册的handler回调函数（irqaction list），所属的irqchip和irqdomain(irq_data)。

所有的irqdesc组成了一个irqdesc列表，并提供了在这个列表中申请irq，获取desc的操作接口。列表可以使用数组储存，也可以使用radix tree方式储存，具体方式由CONFIG_SPARSE_IRQ来控制。默认情况，都是SPARSE方式，也就是用radix tree保存，这可以支持更大的irq列表，同时节约空间。

在中断触发回调时（中断处理过程见irqdomain），会先调用desc->handle_irq这个中断通用处理函数，通用处理流程因为与arch相关，所以由gic driver实现（gic_irq_domain_map）。在这个通用流程中会最终回调到desc->irqactions列表，也就是用户注册的irq回调函数。

## Files

```
- /include/linux/irqdesc.h
- /kernel/irq/irqdesc.c
```

## Variables

`irq_desc[NR_IRQS]`

irqdesc数组（非sparse方式）

`static RADIX_TREE(irq_desc_tree, GFP_KERNEL);`

irqdesc树（sparse方式）

`int nr_irqs = NR_IRQS;`

支持的irq数量

`allocated_irqs`, `#define IRQ_BITMAP_BITS (NR_IRQS + 8196)`

已分配irq bitmap

## Functions

`alloc_descs`

在irqdesc list中申请一个或多个irq，如果virq传-1，则从已分配的irq bitmap中找一块合适的连续irq号出来，创建desc，并加入到tree中，返回irq号。

`irq_to_desc`

通过irq号获得desc

`early_irq_init`, `arch_early_irq_init`

允许arch注册早期irq，会获取arch要注册的irq数量，然后创建irq number和对应的desc，然后调用 `arch_early_irq_init` 完成这些irq的初始化。

`handle_irq_desc`

通过desc来调用irq回调

`generic_handle_irq_desc`

通过desc来调用irq回调（最底层、直接）

`generic_handle_irq`

通过irq来调用irq回调

`generic_handle_domain_irq`

通过hwirq和domain来调用irq回调

## Usage Test

`sysfs`

irqdesc在sysfs的/sys/kernel/irq中，为申请的每个irq号的desc创建了文件节点。

- name：irq name
- type：irq type，level，edge等
- hwirq：hwirq
- actions：callback function
- per_cpu_count：各个核的中断次数统计
- chip_name：interrupt controller（gic）
- wakeup

`kstat_irqs`

desc->kstat_irqs中记录了per cpu的中断次数，这个数据接入到了proc/interrupts中，同时提供了 `kstat_irqs_usr` 接口，允许进程上下文来获取各个中断的统计数据。

`irqaffinity`

desc中可以设置interrupt affinity参数，用来对中断进行绑核。在desc初始化时，会用 `irq_default_affinity` 来作为初始化值，同时，在bootargs中，可以通过设置 `irqaffinity=` 在启动时修改这个默认值。

## Refernece

<https://blog.csdn.net/weixin_41028621/article/details/101753159>