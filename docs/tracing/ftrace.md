# ftrace

## 基本功能

**使能Ftrace**

```
mount -t tracefs nodev /sys/kernel/tracing
```

**启动FTrace**

```
echo 1 > /proc/sys/kernel/ftrace_enabled
echo 1 > tracing_on
```

**获取Event列表**

```
ls events
```

**使能Event**

```
echo sched:sched_switch >> set_event
echo 1 > events/sched/sched_switch/enable
```

**使能Kprobe**

```
echo 'r:myretprobe do_sys_open $retval' >> /sys/kernel/debug/tracing/kprobe_events
```

会在events/kporbes/创建出myretprobe目录节点

使能节点开始抓取：

```
echo 1 > events/kprobes/myretprobe/enable
```

kprobe设置的详细格式见参考：

https://www.kernel.org/doc/Documentation/trace/kprobetrace.txt

**使能Syscall**

CONFIG_FTRACE_SYSCALLS=y

会在events/创建出syscall目录，否则只能看到raw_syscall

**获取Trace**

```
cat trace
cat trace_pipe			# 实时获取
cat trace_pipe > test.txt	# 保存到文件
```

**清空Trace**

```
echo > trace
```

**设置Event的PID**

```
echo <pid> > set_event_pid	# 白名单
echo <pid> > set_event_notrace_pid	# 黑名单
```

**设置使能的CPU**

```
cat tracing_cpumask
echo 0x01 > tracing_cpumask
```

**Per CPU Buffer**

```
cat per_cpu/cpu0/trace
cat per_cpu/cpu0/trace_pipe
```

**设置buffer size**

```
echo 1024 > buffer_size_kb
echo 1024 > per_cpu/cpu0/buffer_size_kb
```

**设置时间戳时钟类型**

```
cat trace_clock
echo uptime > trace_clock
```

**设置Tracing Options**

```
ls options
cat options/xxx
echo 1 > options/xxx
```

详见：<https://www.kernel.org/doc/html/latest/trace/ftrace.html#trace-options>

**抓取最大调用栈**

```
echo 1 > /proc/sys/kernel/stack_tracer_enabled
cat stack_trace
```

**创建独立Ftrace Instance**

```
mkdir instances/test
```

**设置Tracer**

```
cat available_tracer
echo <tracer> > available_tracer
```

## Tracers

### NOP Tracer（Event Tracing）

事件trace，事件触发则打一个点，

**设置tracer**

```
echo nop > current_tracer
```

**抓取事件发生时的调用栈**

```
echo 1 > options/stacktrace
```

之后每个事件打点下面，都会包括这个事件发生时的调用栈

### Function Tracer

函数trace，通过kprobe实现，每个函数进出都会打点，trace所有函数执行过程

**设置tracer**

```
echo  function > current_tracer
```

**获得函数列表**

```
cat available_filter_functions
```

**设置跟踪函数**

```
echo <function> > set_ftrace_filter	# 白名单
echo <function> > set_ftrace_notrace	# 黑名单
```

设置后trace中只包含这些函数。

**设置追踪PID**

```
echo <pid> > set_ftrace_pid# 白名单
echo <pid> > set_ftrace_notrace_pid	# 黑名单
```

**抓取函数调用栈**

```
echo <function> > set_ftrace_filter	# 务必设置，否则要打印的栈太多
echo 1 > options/func_stack_trace
```

**设置filter（不成功）**

```
echo '<function>:traceoff:5' > set_ftrace_filter	# 打印五次后关闭
```

**Snapshot（不成功）**

```
echo '<function>:snapshot' > set_ftrace_filter
echo '<function>:snapshot:1' > set_ftrace_filter	# 只保存一次
```

### Function Graph Tracer

功能与fucntion tracer相同，通过图形树显示整个函数执行过程，及每个函数执行时间

**设置tracer**

```
echo function_graph > current_tracer
```

**设置追踪深度**

```
echo 3 > max_graph_depth
```

**设置追踪起始函数（根函数）**

```
echo <function> > set_graph_function	# 白名单
echo <function> > set_graph_notrace	# 黑名单
```

\* 这个节点设置的是追踪的起始函数，在这些函数开始追踪

**设置跟踪函数、设置追踪PID**

与Function Tracer使用相同功能节点，这个节点过滤的是打点的所有函数

### Other Tracers

<https://www.kernel.org/doc/html/latest/trace/ftrace.html#the-tracers>

## Boot-time tracing

ftrace可以在启动阶段打开，并使用tp_printk参数直接将trace显示到串口上。

**通过bootconfig配置boot-time ftrace**

```
ftrace {
      tracing_on = 0
      tracer = function_graph
      event.kprobes {
              start_event {
                      probes = "pci_proc_init"
                      actions = "traceon"
              }
              end_event {
                      probes = "pci_proc_init%return"
                      actions = "traceoff"
              }
      }
}
```

抓取pci_proc_init到结束间的所有函数调用。

更多配置可以参考：<https://www.kernel.org/doc/html/latest/trace/boottime-trace.html>

**通过bootargs配置boot-time ftrace**

```
trace_options=sym-addr trace_event=initcall:* tp_printk trace_buf_size=1M ftrace=function ftrace_filter="vfs*"

```

抓取initcall事件。

更多配置可以参考：<https://www.kernel.org/doc/html/latest/admin-guide/kernel-parameters.html#kernelparameters>（搜索“FTRACE”）

## Dump ftrace

1、调用ftrace_dump接口dump ftrace，或者在panic时触发dump。

2、查看global_trace全局变量的head_page，应该就是ftrace ringbuffer链表。

## Reference

ftrace

<https://www.kernel.org/doc/html/latest/trace/ftrace.html>

boot-time ftrace

<https://www.kernel.org/doc/html/latest/trace/boottime-trace.html>

?
<https://www.kernel.org/doc/html/latest/trace/ftrace-uses.html>