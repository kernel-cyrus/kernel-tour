# oops

Kernel在发生异常时，会打印出oops日志，oops中包括了问题的现场信息。

如果是严重问题，导致Kernel运行不再可信，则会执行panic，将系统终止运行。

在代码中，我们也可以加入BUG检查，在不应该进入的分支加入WARN_ON或BUG_ON，将BUG report出来。

## Files

```
- /kernel/panic.c
- /include/asm-generic/bug.h
```

## Functions

`panic`

执行panic，打印现场oops，并终止系统运行。内核在很多Exception Handler中，执行了panic()。

`BUG_ON`

判断bug条件，触发panic。

`WARN_ON`

判断bug条件，只report oops，不触发panic。

## Usage Test

### Enalbe more debug info on panic

向cmdline添加panic_print=0xff，使能panic更多打印，参考值在 `panic.c` 中定义。

### Trigger panic in WARN_ON

向cmdline添加panic_on_warn=1，在WARN_ON的地方触发panic

### Stop on first oops

向cmdline添加pause_on_oops=1，会在第一个panic处停止，防止oops刷屏。

## Module Test

[test-oops](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-oops)