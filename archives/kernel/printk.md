# printk

## Printk Interfaces

printk提供了以下接口用来添加内核打印：

**printk**

最基本的printk接口

**pr_xxx**

包含打印级别的打印接口

```
| Name           | String |  Alias function                               |
+================+========+===============================================+
| KERN_EMERG     | "0"    | pr_emerg()                                    |
| KERN_ALERT     | "1"    | pr_alert()                                    |
| KERN_CRIT      | "2"    | pr_crit()                                     |
| KERN_ERR       | "3"    | pr_err()                                      |
| KERN_WARNING   | "4"    | pr_warn()                                     |
| KERN_NOTICE    | "5"    | pr_notice()                                   |
| KERN_INFO      | "6"    | pr_info()                                     |
| KERN_DEBUG     | "7"    | pr_debug() and pr_devel() if DEBUG is defined |
| KERN_DEFAULT   | ""     |                                               |
| KERN_CONT      | "c"    | pr_cont()                                     |
```

显示打印级别：

```
 $ cat /proc/sys/kernel/printk
 4        4        1        7
 current  default  minimum  boot-time-default
```

修改console打印级别：

1、`echo 8 > /proc/sys/kernel/printk`

2、`dmesg -n 5`

3、`loglevel=X`

控制是否进/dev/kmsg：`printk.devkmsg`

**pr_debug**

pr_debug用来添加debug打印，默认不会被开启。

可以通过以下几种方式开启pr_debug的打印：

method 1: add CFLAGS_xxx.o = -DDEBUG in xxx.o Makefile (enable for .c)

method 2: add #define DEBUG in .c (enable for .c)

method 3: add KBUILD_CFLAGS += -DDEBUG in root Makefile (enable all)

method 4: enable CONFIG_DYNAMIC_DEBUG, echo "module xxx" > /sys/kernel/debug/dynamic_debug/control (enable for ko)

**dev_xxx**

包含了device id的打印接口，适用于driver。

**pr_fmt**

由于所有pr_xxx接口都使用了pr_fmt来作为打印格式，可以在.c文件头部定义pr_fmt宏，来修改默认的打印格式，比如：

```
#define pr_fmt(fmt) "%s:%s: " fmt, KBUILD_MODNAME, __func__
```

## Printk Logs

有以下几种方式可以获取printk log：

**devkmsg**

dev中提供了对printk ringbuffer的dump和写入节点：`/dev/kmsg`

`cat /dev/kmsg`

`echo "message" > /dev/kmsg`

**/proc/kmsg**

proc下的kmsg，只要读取过一次kmsg后，就不会再继续显示读取过的，只会显示新增的。

**dmesg**

dmesg可以认为是一个格式化读取、过滤kmsg的工具。

**syslog**

syslog是一个专门用于读取kernel log的syscall，userspace的程序，如dmesg，都是基于这个syscall实现。

## Printk Buffers

printk提供了两个buffer：

`*log_buf` (`__log_buf`)

printk static buffer，一块连续内存，存放pure text log。

Debug启动问题时，经常会Dump这块Buffer。

`*prb`

printk ringbuffer，是一个buffer链表，提供了 `prb_commit`、`prb_final_commit`、`prb_reserve` 等接口用来操作这块buffer。

这类buffer可以简单理解为一个数据库即可。

## Printk Functions

`printk`

```
vprintk_store
	prb_commit or prb_final_commit 	=> add to prb
	prb_reserve -> r.text_buf
	printk_sprint(r.text_buf) 	=> add to log_buf
```

`early_printk`

直接打印到earlycon

`kmsg_dump_register`

可以在oops或panic时，对log进行dump

`call_console_drivers`

调用console driver，向console打印log出来。

## Printk Console Switch

开机阶段，在uart初始化结束后，我们会看到printk打出console从earlycon到console的切换log：

```
printk: console [tty0] enabled
printk: bootconsole [pl11] disabled
```

这个log是在earlycon结束后，uart driver框架`uart_configure_port`调用`register_console`，向printk注册自己打出来的。

## Log Format

`CONFIG_PRINTK_CALLER`

开启Caller后，Log中会打印"C"、"T"，分别代表CPU、ThreadID信息。

有些人需要在每行log显示CPU信息，社区有很多patch实现这一功能，比如：

<https://lore.kernel.org/lkml/1344013004-32211-1-git-send-email-vikram.pandita@ti.com/>

## Files

```
- /include/linux/printk.h
- /include/linux/dev_printk.h
- /kernel/printk/printk.c
```

## Printk Formats

常用数据类型printk format：

```
type							format(DEC)		format(HEX)
--------------------------------------------------------------------------------------------
char / short int / int					%d			%x
unsigned char / unsigned short int / unsigned int	%u			%x
long							%ld			%lx
unsigned long						%lu			%lx
long long						%lld			%llx
unsigned long long					%llu			%llx
s8 / s16 / s32						%d			%x
u8 / u16 / u32						%u			%x
s64							%lld			%llx
u64							%llu			%llx
size_t							%zu			%zx
ssize_t							%zd			%zx
void*							-			%p
phys_addr_t						-			%pa[p]
dma_addr_t						-			%pad
```

## Reference

<https://www.kernel.org/doc/html/latest/core-api/printk-basics.html>

<https://zhuanlan.zhihu.com/p/521094976>