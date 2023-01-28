# wakelock

wakelock是睡眠锁，只要有thread持锁，就会阻止系统进入suspend。

内核态可以通过调用`pm_wake_lock`、`pm_wake_unlock`申请释放锁。

用户态可以通过写入`/sys/power/wake_unlock`，`/sys/power/wake_lock`节点申请释锁。

参数buf的格式为"name timeout"

如果存在同名的wakelock，则覆盖，不存在则创建。

wakelock底层调用wakeup的pm_stay_awake，将suspend禁用。

## Files

```
- /kernel/power/wakelock.c
```

## Reference

<http://www.manongjc.com/detail/56-ihbqqtiotlyqbqp.html>
