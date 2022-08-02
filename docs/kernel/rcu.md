# rcu

背景：多线程下如何操作一个链表？

链表再被多线程读取时，不会有任何问题，但是当有人去改写链表时，就可能带来问题。

我们直观想到的方法是在每次操作时，给整个链表上锁。但是，对于那些大部分情况都是用于读取的链表，仅仅因为级小概率的改写，就锁定整个链表，这种方法大大影响了链表的读取效率。

RCU是内核实现的多线程操作链表的新机制，通过RCU接口及RCU的链表访问流程，可以在允许链表修改的同时，大大提高链表的读效率。

RCU机制将链表访问者分为：Reader / Updater

将链表操作分为：Insdert，Remove，Update

RCU的大体流程，就是在Updater去Update链表时，先将修改写入Node的一份Copy，在等到这个Node被所有Reader释放后，再完成Copy和原始Node的替换，完成更新过程。

这种机制类似于一种注册机制，Updater作为其所更新Node的Lisener，等待作为User的Reader使用完释放Node。

实际上，RCU是一种锁机制之外的解决并发访问的通用方法，只是大部分情况下，我们只需要使用rculist这种rcu的典型应用场景。

## Files

```
- /kernel/rcu/*			// different RCU
- /include/linux/rcupdate.h	// RCU implementation
- /include/linux/rculist.h	// RCU list implementation
```

## Interface

`rcu_read_lock()`

进入read-side critical section

`rcu_read_unlock()`

退出read-side critical section

`synchronize_rcu()` / `call_rcu()`

停等或回调，在所有read-side critical section退出后，publish所有update，并（直接或者在回调中）回收内存

`rcu_assign_pointer()`

对某个pointer进行Update Copy操作，在所有read-side critical section退出后，将pointer修改为新的update copy的地址

`rcu_dereference()`

在read-side critical section中，解引用一个pointer

## Module Test

[test-rcu](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-rcu)

## Reference

<https://zhuanlan.zhihu.com/p/113999842>

<https://www.kernel.org/doc/html/latest/RCU/whatisRCU.html>

<https://lwn.net/Articles/262464/>

<https://www.kernel.org/doc/Documentation/RCU/listRCU.txt>