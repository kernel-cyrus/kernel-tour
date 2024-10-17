# irq/procfs

**/proc/interrupts**

generic show interrupts：

显示每个online cpu的中断统计，irq chip，hwirq，level type

按gic定义，其中0\~15是SGI中断，16\~31是PPI中断，32以上是SPI中断。

arch show interrupts：

IPI中断（核间通信中断）。ARM平台，每个消息类型使用了一个SGI中断（IPI消息类型见ipi_msg_type）。

IPI 0\~6，使用了SGI 0\~6，对应了irq 1\~7。

**/proc/default_smp_affinity**

设置中断default afiinity（cpumask）

**/proc/irq/<irq>**

- smp_affinity：irq affinity（ff）

- smp_affinity_list：irq affinity（0-7）

- spurious：显示未被处理的中断，只有有未被处理中断发生时，那个irq count才会开始计数，否则都是0（见spurious.c/note_interrupt）

- node：numa node, always 0

- affinity_hint：? irq balance...

- effective_affinity：?

- effective_affinity_list：?

# irq/sysfs

**/sys/kernel/irq/\<irq\>**

- actions：IPI？

- hwirq：硬中断号

- per_cpu_count：中断数量

- wakeup：？

- chip_name：中断控制器名

- name：中断名

- type：中断level类型

# irq/debugfs

**/sys/kernel/debug/domains/\<domain\>**

dump irq domain信息，包括irq domain名字，中断总量，mapped数量，flag

**/sys/kernel/debug/irqs/\<irq\>**

dump irq信息，包括handler，domain，chip，hwirq，flag，state，affinity