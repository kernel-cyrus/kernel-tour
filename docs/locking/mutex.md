# mutex

mutex是在进程上下文中使用的睡眠锁。

mutex的结构体中，保存了正在持锁的进程（->owner），在没被申请时为NULL

由于mutex在睡眠时会经过上下文切换，代价较大，在后面的mutex实现中，经过了几轮优化，现在的mutex在申请时有三条路径：

- fastpath：直接申请成功
- midpath：如果lock owner正在running（被认为很快会释放），又没有其他进程在等锁（马上会轮到）则会进入spinlock状态
- slowpath：进程进入TASK_UNINTERRUPTIBLE状态，进入wait-queue，等待锁释放后被唤醒。

所以，睡眠锁并不一定真的会睡眠。

最后，mutex只能用于进程上下文，不能用于软中断、硬中断上下文，比如tasklet、timers。

## Files

```
- /include/linux/mutex.h		# mutex interface
- /kernel/locking/mutex.c		# mutex implementation
- /kernel/locking/mutex-debug.c		# CONFIG_DEBUG_MUTEXES
```

## Interface

`DEFINE_MUTEX(name)`

静态定义mutex

`mutex_init(mutex)`

动态初始化mutex

`mutex_lock`

申请锁，禁用中断

`mutex_lock_interruptible`

申请锁，允许中断

`mutex_unlock`

释放锁

`mutex_trylock`

申请锁，否则返回，不等待

`mutex_is_locked`

返回锁是否locked

## Debug Test

开启mutex debug

`CONFIG_DEBUG_MUTEXES=y`

开启后，回去在锁申请和释放时自动检查各种mutex约束条件，如重复初始化，嵌套等。

## Module Test

[test-mutex](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-mutex)

## Reference

<https://docs.kernel.org/locking/mutex-design.html>