# semaphore

信号量是一种进程间的同步机制，与spinlock的区别在于只能用于进程上下文，与mutex的区别在于他可以允许critical section同时被n个进程并行。

spinlock和mutex在多线程编程中多用于互斥，而信号量则多用于同步（如生产者消费者模型）

信号量竞争也纳入到了lockdep机制中，打开lockdep相关debug选项后，信号量导致的死锁也可以被检测出来。

## Files

```
- /include/linux/semaphore.h
- /kernel/locking/semaphore.c
```

## Functions

`sema_init`

动态初始化信号量

`DEFINE_SEMAPHORE`

静态定义信号量，不推荐，使用前看下定义。

`down`

获取信号量（-1），或者睡下去，不允许任何signal唤醒，一直等待up的释放唤醒。

`down_interruptible`

同上，如果睡眠过程中被signal唤醒，则返回-EINTR，正常等到up唤醒返回0

`down_killable`

同上，不允许普通signal唤醒，如果睡眠过程中被faltal signal唤醒，则返回-EINTR，正常等到up唤醒返回0

`down_trylock`

获取成功返回0，没申请到不睡眠，直接返回1

`down_timeout`

获取信号量，如果等了n jiffies都没获取到则返回-ETIME，获取到正常等到up释放唤醒则返回0

`___down_common`

上面所有down函数都是调用 `___down_common` 实现的，`___down_common` 函数核心是调用 `schedule_timeout` 睡眠下去，一直等 `up` 唤醒。

其中，`signal_pending_state` 在当前进程状态可以接受信号量并且存在PENDING信号量的情况下，返回TRUE，其他情况返回FALSE。

`up`

释放信号量（+1），并唤醒信号量等待进程队列中的第一个进程。

## Module Test

[test-semaphore](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-semaphore)

## Reference

<https://0xax.gitbooks.io/linux-insides/content/SyncPrim/linux-sync-3.html>