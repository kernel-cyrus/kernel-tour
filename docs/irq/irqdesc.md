# irqdesc


## Files

```
- /include/linux/irqdesc.h
- /kernel/irq/irqdesc.c
```

## Variables

`irq_desc[NR_IRQS]`


## Functions

`early_irq_init`


`arch_early_irq_init`


`alloc_desc`

`alloc_descs`


`irq_to_desc`

`generic_handle_irq`

`generic_handle_irq_desc`

`handle_irq_desc`

`generic_handle_domain_irq`





`desc_smp_init`

初始化desc的affinity，没传值用默认的irq_default_affinity，默认affinity可以通过bootargs `irqaffinity=` 进行设置。

## Usage Test

sysfs

kstat_irqs

## Refernece

<https://blog.csdn.net/weixin_41028621/article/details/101753159>