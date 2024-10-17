# lockstat

lockstat 提供了全局的lock统计信息，可以对全系统的所有锁类竞争情况，持锁情况进行统计。

## Files

```
- /kernel/locking/lockdep.c             // CONFIG_LOCK_STAT
```

## Usage Test

1、使能

Enable `CONFIG_LOCK_STAT` 

2、开启/关闭

```
echo 1 >/proc/sys/kernel/lock_stat
echo 0 >/proc/sys/kernel/lock_stat
```

3、查看

```
less /proc/lock_stat
```

4、阅读

```
 01 lock_stat version 0.4
  02-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  03		class name    con-bounces    contentions   waittime-min   waittime-max waittime-total   waittime-avg    acq-bounces   acquisitions   holdtime-min   holdtime-max holdtime-total   holdtime-avg
  04-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  05
  06	&mm->mmap_sem-W:            46             84           0.26         939.10       16371.53         194.90          47291        2922365           0.16     2220301.69 17464026916.32        5975.99
  07	&mm->mmap_sem-R:            37            100           1.31      299502.61      325629.52        3256.30         212344       34316685           0.10        7744.91    95016910.20           2.77
  08	---------------
  09	&mm->mmap_sem              1          [<ffffffff811502a7>] khugepaged_scan_mm_slot+0x57/0x280
  10	&mm->mmap_sem             96          [<ffffffff815351c4>] __do_page_fault+0x1d4/0x510
  11	&mm->mmap_sem             34          [<ffffffff81113d77>] vm_mmap_pgoff+0x87/0xd0
  12	&mm->mmap_sem             17          [<ffffffff81127e71>] vm_munmap+0x41/0x80
  13	---------------
  14	&mm->mmap_sem              1          [<ffffffff81046fda>] dup_mmap+0x2a/0x3f0
  15	&mm->mmap_sem             60          [<ffffffff81129e29>] SyS_mprotect+0xe9/0x250
  16	&mm->mmap_sem             41          [<ffffffff815351c4>] __do_page_fault+0x1d4/0x510
  17	&mm->mmap_sem             68          [<ffffffff81113d77>] vm_mmap_pgoff+0x87/0xd0
  18
  19..........................................................................................................................................................................................................
  20
  21	unix_table_lock:           110            112           0.21          49.24         163.91           1.46          21094          66312           0.12         624.42       31589.81           0.48
  22	---------------
  23	unix_table_lock             45          [<ffffffff8150ad8e>] unix_create1+0x16e/0x1b0
  24	unix_table_lock             47          [<ffffffff8150b111>] unix_release_sock+0x31/0x250
  25	unix_table_lock             15          [<ffffffff8150ca37>] unix_find_other+0x117/0x230
  26	unix_table_lock              5          [<ffffffff8150a09f>] unix_autobind+0x11f/0x1b0
  27	---------------
  28	unix_table_lock             39          [<ffffffff8150b111>] unix_release_sock+0x31/0x250
  29	unix_table_lock             49          [<ffffffff8150ad8e>] unix_create1+0x16e/0x1b0
  30	unix_table_lock             20          [<ffffffff8150ca37>] unix_find_other+0x117/0x230
  31	unix_table_lock              4          [<ffffffff8150a09f>] unix_autobind+0x11f/0x1b0
```

```
con-bounces     - The number of contended lock acquisitions which involved cross-cpu data.
contentions     - 等锁次数
waittime-min    - 最低等待时间(us)
waittime-max    - 最大等待时间(us)
waittime-total  - 总等待时间(us)
acq-bounces     - The number of lock acquisitions which involved cross-cpu data.
acquisitions    - 持锁次数
holdtime-min    - 最低持锁时间(us)
holdtime-max    - 最大持锁时间(us)
holdtime-total  - 总持锁时间(us)
```

表中包含两组锁类，每组为一个锁类的统计，每组分为三部分，第一部分为锁类的整体统计，第二部为四处Try to get the lock的位置，第三部分为四处Lock holder的位置，这几处位置并不是全部数据，可能丢失其他位置。


5、竞争最激烈的十个锁类

```
grep : /proc/lock_stat | head
```

## Reference

<https://docs.kernel.org/locking/lockstat.html>

<https://groups.google.com/g/linuxkernelnewbies/c/6BJYVrHHh8U>