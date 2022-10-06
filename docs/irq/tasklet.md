# tasklet

tasklet是一种软中断，准确说，tasklet使用了两个软中断（HI_SOFTIRQ / TASKLET_SOFTIRQ），来允许用户注册自己的中断下半部处理函数到软中断中。

tasklet在设计上，不同类型tasklet（不同的tasklet对象）允许多核并行，同一类型采用了多核串行的方式（同一个tasklet对象），不需要考虑多核并行重入的问题，这就避免像softirq一样，必须要为多核并行上锁，导致下半部处理函数变得更加复杂。

## Files

```
- /kernel/softirq.c			# tasklet implementation
- /include/linux/interrupt.h		# tasklet interface
```

## Variables

`tasklet_vec`, `tasklet_hi_vec`

两个不同优先级tasklet全局链表。percpu，每个cpu都有独立链表。

## Functions

`softirq_init`

初始化两个tasklet链表，并初始化其所使用的两个softirq，绑定softirq回调函数

`tasklet_action`, `tasklet_hi_action`

两个tasklet软中断处理回调函数，会最终调用 `tasklet_action_common`

`tasklet_action_common`

tasklet执行函数，在执行tasklet时，会去判断这个tasklet有没有上锁（处理中），如果有锁，则等待下次处理。这里，尽管每个CPU有独立的tasklet链表，但是链表中的tasklet对象可能是同一个（创建tasklet时并没有为每个核创建独立的percpu tasklet），这种情况下，同一个tasklet就可能在两个不同核发生中断时，都被添加到tasklet列表，出现这种串行的情况。

`DECLARE_TASKLET`, `DECLARE_TASKLET_DISABLED`

tasklet静态定义函数，创建tasklet对象并绑定回调函数

`tasklet_init`, `tasklet_setup`

tasklet动态初始化函数，为tasklet对象初始化回调函数

tasklet支持两种prototype的回调，分别使用两个接口初始化，两种类型的回调都可以使用。

`tasklet_schedule`, `tasklet_hi_schedule`

将tasklet加入到当前CPU的对应优先级tasklet链表，raise对应链表的softirq，等待处理。

`tasklet_enable`, `tasklet_disable`

使能/禁用tasklet，会等待tasklet处理完毕。

## Reference

https://blog.csdn.net/oqqYuJi12345678/article/details/99771140