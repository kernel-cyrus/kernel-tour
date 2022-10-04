# manage

manage包括了request（注册）、enable（使能）、affinity（绑核）等操作，主要处理irq的回调注册。

## Files

```
- /kernel/irq/manage.c
```

## Functions

`synchronize_irq`

等待某路中断处理结束（包括中断及中断处理线程）

`synchronize_hardirq`

等待某路中断处理结束（包括中断处理函数）

`irq_set_affinity`

设置某路中断绑核。affinity系函数中，除了可以设置硬中断的affinity，还可以设置中断处理线程的affinity。

`enable_irq`

使能某路中断

`disable_irq`

禁用某路中断，等待所有中断处理结束（包括中断及中断处理线程）

`disable_irq_hardirq`

禁用某路中断，等待所有中断处理结束（包括中断处理函数）

`request_irq`

中断申请接口，为irq绑定回调函数

`setup_irq_thread`, `irq_thread`

中断线程化机制，为每个中断创建独立的中断后半部处理线程

类似workqueue，可以将中断后半部，甚至全部中断处理过程放到thread中执行，与workqueue不同的是，workqueue是一个线程来处理各个中断的下半部work，而threaded irq是为每个中断的下半部创建了独立的thread。threaded irq尽管量级更重一些，但是在下半部比较大的时候，threaded irq在调度上更为灵活，允许下半部被调度到其他核处理（可以由threaded affinity控制），可以为系统提供更大的处理带宽。

`request_threaded_irq`

中断（线程化）申请接口，为irq绑定回调及下半部处理函数

`request_percpu_irq`

中断申请接口，为local中断（本CPU中断）绑定回调函数。

`__setup_irq`

底层中断绑定接口，各种request申请接口都使用此接口做最后的安装和绑定