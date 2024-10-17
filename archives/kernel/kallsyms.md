# kallsyms

开发者为了能够更方便调试内核问题，在内核镜像中加入了一个运行时可访问的符号表，并提供了对这个符号表的访问和查询接口，这就是kallsyms。

kallsyms使用一个私有数据块来保存符号信息，在编译时被链接到了内核里，以只读全局变量的形式存在。kallsyms提供了访问和查询这个符号表全局变量的接口。这个符号表不是vmlinux中elf的符号表。

在Kernel oops中，会显示stack backtrace，其中显示的符号信息，就是利用kallsyms实现的。

## Files

```
- /include/linux/kallsyms.h
- /kernel/kallsyms.c
- /kernel/module/kallsyms.c
```

## Functions

`printk`

printk中可以通过%px显示指针地址，通过%ps或%pS（包含offset/size信息）显示指针对应的符号。

`print_ip_sym`

printk的封装，便捷打印addr对应符号位置。

`kallsyms_lookup` `lookup_symbol_name`

通过addr查找symbol，包括在symbol的size和symbol内的偏移

`kallsyms_lookup_name`

查找symbol对应的地址

`dump_stack`

打印当前调用栈

## Usage Test

**使能kallsyms符号表**

CONFIG_KALLSYMS=y，CONFIG_KALLSYMS_ALL=y

**查看kallsyms符号表**

cat /proc/kallsyms

- b 符号在未初始化数据区（BSS）
- c 普通符号，是未初始化区域
- d 符号在初始化数据区
- g 符号针对小object，在初始化数据区
- i 非直接引用其他符号的符号
- n 调试符号
- r 符号在只读数据区
- s 符号针对小object，在未初始化数据区
- t 符号在代码段
- u 符号未定义

\* 使用root权限查看

## Module Test

[test-kallsyms](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-kallsyms)

## Related: dump_stacks / stacktrace

dump stack提供了stack backtrace相关接口，详见 `dump_stack.c` `stacktrace.c`

可以通过 `dump_stack` 打印当前调用栈，通过 `arch_stack_walk` 对stack进行遍历，通过传入 `task_struct` 打印线程栈。