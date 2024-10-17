# rwsem

并发的很多场景中，由于大多数内核临界区，都是在做读访问，写访问很少，如果不区分读写都对临界区上锁，就降低了效率。

为了解决这类访问场景，有了rwsem(读写信号量)。rwsem的临界区有以下几个约束：

1. 允许多个reader进入临界区，数量由信号量n决定，n消耗完后，其余reader进入睡眠等待
2. 只允许一个writer进入临界区，有writer持锁时，其他reader、writer均进入睡眠等待
3. reader、writer不能同时进入临界区，也就是说，有reader持锁时，writer进入睡眠等待

rwsem与semaphore一样，只用于进程上下文，等锁进程会进入睡眠。

实现上，rwsem有一个list来维护等待的进程队列，同时在writer持锁时，owner会指向持锁进程。count会保存信号量的值并用高位反应出持锁状态信息。

## Files

```
- /include/linux/rwsem.h
- /kernel/locking/rwsem.c
```

## Interfaces

`DECLARE_RWSEM`

静态定义初始化

`init_rwsem`

动态初始化

`down_read`

获取信号量（-1）,如果write lock，则睡眠

`down_read_interruptible`

同上，如果睡眠过程中被signal唤醒，则返回-EINTR，正常等到up唤醒返回0

`down_read_killable`

同上，不允许普通signal唤醒，如果睡眠过程中被faltal signal唤醒，则返回-EINTR，正常等到up唤醒返回0

`down_read_trylock`

获取成功返回0，没申请到不睡眠，直接返回1

`down_write`

获取信号量（-1）,如果有read / write lock，则睡眠

`down_write_killable`

同上，不允许普通signal唤醒，如果睡眠过程中被faltal signal唤醒，则返回-EINTR，正常等到up唤醒返回0

`down_write_trylock`

获取成功返回0，没申请到不睡眠，直接返回1

`up_read` `up_write`

释放信号量（+1），并唤醒信号量等待进程队列中的第一个进程。

## Module Test

[test-semaphore](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-semaphore)

## Reference

<https://0xax.gitbooks.io/linux-insides/content/SyncPrim/linux-sync-5.html>

<https://www.cnblogs.com/hellokitty2/p/16343272.html>