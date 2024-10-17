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

`enable_irq`, `disable_irq`

使能/禁用某路中断，等待所有中断处理结束（包括中断及中断处理线程）

调用的是irq chip的底层回调，操作的是gic寄存器，屏蔽了某路中断。

`enable_percpu_irq`, `disable_percpu_irq`

使能/禁用percpu申请的中断，与上面函数类似，屏蔽的是gic某路中断。

`disable_irq_nosync`

与上面函数类似，但是不等待所有中断处理结束。

`local_irq_enable`, `local_irq_disable` (irqflags.h)

禁用当前CPU中断，操作的是本CPU的daif寄存器，屏蔽了中断响应，但是并没有屏蔽GIC的中断发送，新来的中断还是会发送给PE，并且PENDING住。

这组接口就是直接的开关，并不会记录嵌套层级。

`local_irq_save`, `local_irq_restore` (irqflags.h)

与上面接口类似，但是会记录嵌套层级，在最后的restore时，才会打开中断响应。

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

percpu中断申请接口（PPI），为local中断（本CPU中断）绑定回调函数。

`__setup_irq`

底层中断绑定接口，各种request申请接口都使用此接口做最后的安装和绑定

## Reference

关于禁中断的接口比较：<http://books.gigatux.nl/mirror/kerneldevelopment/0672327201/ch06lev1sec7.html>