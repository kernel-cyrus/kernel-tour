# spin table

在主CPU（Primary CPU）启动后，从核（统称Secondary CPU）有两种启动方式，一种通过PSCI，另一种通过Spin-Table。

**PSCI**

详细配置见"PSCI"，从核的上电由ATF完成。

**Spin-Table**

从核上电由Bootloader完成，Secondary CPU在Bootloader初始化后通过WFE Spin在一个地址（cpu-release-addr）。

主核启动后，向cpu-release-addr写入从核启动地址，并通过SVE通知从核解锁。

从核解WFE，判断cpu-release-addr解Spin，跳转到传入地址开始进入Kernel Boot Secondary CPU流程。

**DTS**

`enable-method`

DTS CPU节点的enable-method可以配置为spintable和psci。

如果使用spintable，则使用spintable实现的cpu_ops来做CPU的电源管理。这部分代码在smp_spin_table.c中实现，主要用于secondary cpu的启动。

`cpu-release-addr`

cpu-release-addr，一块内存地址，内容是cpu release后跳转的地址。

primary cpu启动后，在boot secondary阶段，会从dts cpu-release-addr读取spin的地址，并将内核从核启动函数地址写入这块内存，并执行sve发送signal给从核。

secondary cpu启动后，会进入wfe，并不断轮训cpu-release-addr的内容，当内容有值时，则跳入到这个地址运行。

**Spin-table: cpu_ops**

smp_spin_table_ops包括cpu_init, cpu_prepare, cpu_boot三个回调实现。

- cpu_init：负责从DTS CPU节点读取cpu-release-addr的地址，保存到cpu_release_addr\[cpu\]中。

- cpu_prepare：把kernel secondary boot函数地址写入这块spin内存

- cpu_boot：sve唤醒从核启动

## Files

```
- /arch/arm64/kernel/smp_spin_table.c		# Spin-table Implementation
```

## Reference

<https://zhuanlan.zhihu.com/p/537049863>