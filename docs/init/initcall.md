# initcall

initcall是builtin的module启动阶段初始化的机制。

initcall提供了不同的优先级，允许module通过initcall接口把初始化函数添加到某个优先级初始化列表中。

内核启动阶段做 `do_initcalls` 时，会按照优先级顺序遍历每个initcall列表，逐一执行module的初始化函数。

如果module在编译时不是builtin的方式，而是编译为ko，则initcall接口会被替换为module_init。

initcall的列表在实现上被静态编译到了内存段中，见 `do_initcalls` 实现。

## Files

/include/linux/init.h

## Interfaces

```
#define early_initcall(fn)		__define_initcall(fn, early)
#define pure_initcall(fn)		__define_initcall(fn, 0)
#define core_initcall(fn)		__define_initcall(fn, 1)
#define core_initcall_sync(fn)		__define_initcall(fn, 1s)
#define postcore_initcall(fn)		__define_initcall(fn, 2)
#define postcore_initcall_sync(fn)	__define_initcall(fn, 2s)
#define arch_initcall(fn)		__define_initcall(fn, 3)
#define arch_initcall_sync(fn)		__define_initcall(fn, 3s)
#define subsys_initcall(fn)		__define_initcall(fn, 4)
#define subsys_initcall_sync(fn)	__define_initcall(fn, 4s)
#define fs_initcall(fn)			__define_initcall(fn, 5)
#define fs_initcall_sync(fn)		__define_initcall(fn, 5s)
#define rootfs_initcall(fn)		__define_initcall(fn, rootfs)
#define device_initcall(fn)		__define_initcall(fn, 6)
#define device_initcall_sync(fn)	__define_initcall(fn, 6s)
#define late_initcall(fn)		__define_initcall(fn, 7)
#define late_initcall_sync(fn)		__define_initcall(fn, 7s)
```

## Usage

**Enable initcall debug during boot**

cmdline添加"initcall_debug=1 loglevel=9"，会打印所有执行的initcall。