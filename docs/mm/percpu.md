# percpu

在SMP系统中，变量是被多核所共享使用的，percpu允许为每个CPU定义私有变量。

percpu提供静态定义和动态分配两种私有变量的定义方法，静态方法将变量定义在kernel image的percpu section中，动态方式则使用正常的内存分配机制为每个cpu分配出内存。

两种方式都会定义出NR_CPU个变量（每个cpu一个），所以percpu提供的定义或者alloc接口，实际分配出的是一个数组(CPU Number)。在访问某个CPU对应的私有变量时，通过percpu变量的基地址+cpu的offset，最终得到该CPU对应的变量。percpu提供了接口来访问各自CPU私有数据。

最后，percpu分配的变量，一般是CPU的私有数据，一般情况不应该跨CPU访问其他CPU的私有变量，所以自己访问自己的变量是不需要加锁的，因为不会有其他核发生并发访问，但是需要在操作时禁用抢占（preempt），防止获得CPU X的变量后，被调度到CPU Y上，出现CPU Y修改CPU X变量的情况。

所幸，percpu提供了get/put的标准流程，来操作percpu定义的变量，get会关闭抢占，put则会打开抢占，这就需要get/put中间的操作过程足够简洁，避免长时间禁用抢占。

## Files

```
- /include/linux/percpu.h
- /include/linux/percpu-defs.h
- /mm/percpu.c
```

## Interface

`DEFINE_PER_CPU(type, var)`

静态方式定义percpu变量。

`EXPORT_PER_CPU_SYMBOL(var)`, `EXPORT_PER_CPU_SYMBOL_GPL(var)`

将静态定义变量导出。

`DECLARE_PER_CPU(type, var)`

外部引用声明。

`get_cpu_var(var)` `put_cpu_var(var)`

使用/结束访问当前CPU变量，会关闭抢占，并根据当前CPU ID自动找到CPU offset对应的私有变量。通过&get_cpu_var获得原变量指针。

`alloc_percpu(type)` `free_percpu(ptr)`

动态申请percpu变量。所定义的变量需要用__percpu修饰，如 `int __percpu *percpu_data`

`per_cpu_ptr(ptr, cpu)`

通过percpu变量获得其CPU私有变量

`raw_cpu_ptr(ptr)` `this_cpu_ptr(ptr)`

通过percpu变量获得当前CPU私有变量，相当于 `per_cpu_ptr(ptr, raw_smp_processor_id())`

## Debug Test

**Alloc Stat**

可以开启 `CONFIG_PERCPU_STATS` 功能，并访问 `pcpu_stats` 全局变量，来获取percpu的全局统计信息，比如当前alloc的数量，最大alloc的size。

**Trace Event**

```
events/percpu/alloc_percpu
events/percpu/percpu_alloc_percpu_fail
events/percpu/percpu_free_percpu
events/percpu/percpu_create_chunk
events/percpu/percpu_destroy_chunk
```

## Module Test

[test-percpu](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-percpu)

## Reference

<https://lwn.net/Articles/22911/>

<https://zhuanlan.zhihu.com/p/363969903>

<https://0xax.gitbooks.io/linux-insides/content/Concepts/linux-cpu-1.html>
