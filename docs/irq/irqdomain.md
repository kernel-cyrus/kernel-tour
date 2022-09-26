# irqdomain

irqdomain就是一个interrupt controller对象，主要的功能是对hwirq和virq创建映射

在中断控制器驱动中（如gicv3），通过 `irq_domain_create` 来创建出一个irqdomain，并通过irq_domain_ops来绑定这个中断控制器的控制接口，如virq<->hwirq，activate irq，deactivate irq等操作。

### irqdomain virq <->hwirq mapping

Linux系统全局为每个中断定义了一个唯一的virq中断号，virq可以通过 `irq_alloc_desc` 来动态申请，所有Linux层的中断操作，回调绑定，都是面向virq的。

实际中断控制器，使用的都是hwirq号（比如从iar寄存器读出来的），在exception发生，从寄存器读出hwirq号，到从domain中找到virq号，再调用virq对应的回调函数，需要irqdomain对hwirq和virq做一个映射(mapping)。

irqdomain提供了几种映射类型：simple, linear, tree

linear的mapping是一个固定256 size的table，在创建domain时，所有的desc都分配并直接映射好。

tree的mapping是一个radix tree，每个virq与desc都是动态分配，动态绑定（加入到mapping tree里的）

### irqdomain hierarchy

irqdomain可以通过结构体中的parent指针，组成一个irqdomain分层结构，这种结构用来对应硬件的中断控制器级联。

irqdomain提供了这种级联结构的控制支持。

### of_phandle_args, fwspec & fwnode

两个结构主要保存了interrupt controller的device tree节点信息（fwnode）和node中的interrupt信息（fwspec args），区别在于原来的of_phandle_args写明了是针对device tree的操作，而fwspec更加抽象，更具有通用性，可以面向更多平台。

有了面向device tree的数据结构，就可以直接定义出面向该数据结构的irq创建或者绑定操作的结构。

另外需注意，device tree中定义的interrupt irq号，既不是virq也不是hwirq，是hwirq类型（比如spi，ppi）内部index。virq为动态申请，hwirq是死的，index->hwirq通过irqdomain->translate接口完成，virq->hwirq通过mapping完成

## Files

```
- /include/linux/irqdomain.h
- /kernel/irq/irqdomain.c
- /arch/arm64/kernel/entry.S		# Exception Vector Table
- /arch/arm64/kernel/entry-common.c	# Exception Handler
- /drivers/irqchip/irq-gic-v3.c		# GICv3 Interrupt Controller Driver
```

## Variables

`struct irq_domain *irq_domain_list`

全局irq domain list，所有注册的irqdomain都在这里。

## Structures

```
struct irq_domain_ops
	match
	select		is fwspec(interrupt) belongs to this domain(controller)
	map
	unmap
-------- hierarchy support --------
	alloc		map a virq to fwspec, init virq desc
	free
	activate	activate irq
	deactivate	deactivate irq
	translate	trans fwspec to hwirq
-------- hierarchy support --------
	debug_show
```

## Functions

`irq_domain_create_linear,tree`

创建irqdomain，绑定irq domain ops，并注册到系统

`irq_create_mapping`

为hwirq创建出virq

`irq_create_of_mapping`

为of_phandle_args创建出virq

`irq_create_fwspec_mapping`, 

为fwspec(device tree node's interrupt seciton)创建出virq

`irq_domain_associate`， `irq_domain_associate_many`

创建virq到hwirq的mapping

`irq_find_mapping`, `irq_resolve_mapping`

找到hwirq对应的virq

`irq_find_matching_fwspec`

get irqdomain from fwspec，其实就是找到fwspec对应的parent controller，再找到domain

`irq_domain_xxx`

xxx为irq_domain_ops中定义的回调函数名，实际这些函数会回调到ops中对应的函数。

这类实际定义的就是类函数。

`__irq_domain_alloc_irqs`

?

`irq_domain_alloc_descs`

?

## Flows

### Create irqdomain (gic-v3 driver init)

**1、GICv3注册**

```
// irq-gic-v3.c
IRQCHIP_DECLARE(gic_v3, "arm,gic-v3", gic_of_init);
```

IRQCHIP_DECLARE将所有IRQCHIP定义添加到了vmlinux的section中，这个section是一个类似initcall的list，会在start_kernel->init_IRQ->irqchip_init时被遍历，并根据dts中的interrup_controller匹配执行初始化（类似platform device probe）

**2、GICv3初始化**

`gic_of_init`, `gic_init_bases`

主要从dts中读取interrupt controller信息，配置寄存器，对GIC进行初始化。

**3、创建irqdomain**

```
gic_data.domain = irq_domain_create_tree(...)
```

gic所有私有数据保存在gic_data全局变量中，初始化时会创建出tree irq domain。

**4、绑定irq_handler**

ARM Core可以相应四类Exception类型（exception，irq，fiq，serror），中断向量表（vector）在entry.S中定义。每个异常类型的处理函数，在exception.h中定义，在entry-common.c中实现。

GIC发送到ARM Core的中断，可以分为irq和fiq，在Core收到exception时，都会路由到vector对应的entry入口函数中。

每个el的所有irq、firq，由系统全局唯一的回调函数handle，这个回调函数就是在gic初始化阶段，通过set_handle_irq设置的，gic-v3为 `gic_handle_irq` `gic_handle_fiq` 函数。

**5、使能GIC**

全部配置后，GIC driver会把中断使能起来。

### Add new irq & handler

ARM平台大多数中断，中断号在DTS中定义，在驱动初始化时，驱动会从自己dts节点中遍历配置的中断号，为中断绑定上驱动的回调函数。

内核针对这种为dts interrupt绑回调的场景，定义了一步到位的irq添加接口 `of_irq_get` （会调用irq_create_of_mapping），可以直接为device node中的第index（dts列表中的第i个）个interrupt创建并返回virq。

然后通过 `request_irq`, `request_percpu_irq` 这类接口，为virq号注册上回调函数。

### Interrupt handling from hwirq -> virq

当中断发生时，首先会进入CPU异常向量表，路由到对应EL的回调中，再跳转到gic注册的全局irq、fiq handler中。在handler中，gic会从iar寄存器读取hwirq中断号，设置ack，然后从自己domain的mapping中找到hwirq对应的virq，最终功能跳入irq注册的回调函数执行中断处理流程。

\* pt_regs is registers stored in exception stack

## Debug Test

**Dump all irqdomain from debugfs**

/debug/domains/中可以dump所有irqdomain，cat可以显示每个domain的name、size、mapcount。

## Reference

<https://blog.csdn.net/weixin_41028621/article/details/101448227>