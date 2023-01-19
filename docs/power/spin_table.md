# spin table


**psci vs spin_table**

DTS CPU节点的enable-method可以配置为spintable和psci，如果使用psci，则follow上面流程。

如果使用spintable，则使用spintable实现的cpu_ops来做CPU的电源管理。这部分代码在smp_spin_table.c中实现，主要用于secondary cpu的启动。


cpu-release-addr，一块内存地址，内容是cpu release后跳转的地址。

primary cpu启动后，在boot secondary阶段，会从dts cpu-release-addr读取spin的地址，并将内核从核启动函数地址写入这块内存，并执行sve发送signal给从核。

secondary cpu启动后，会进入wfe，并不断轮训cpu-release-addr的内容，当内容有值时，则跳入到这个地址运行。

smp_spin_table_ops包括cpu_init, cpu_prepare, cpu_boot三个回调实现。

cpu_init：负责从DTS CPU节点读取cpu-release-addr的地址，保存到cpu_release_addr\[cpu\]中。
cpu_prepare：把kernel secondary boot函数地址写入这块spin内存
cpu_boot：sve唤醒从核启动
