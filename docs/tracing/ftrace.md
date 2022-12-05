Ftrace basic functions

Tracers...including hwlat, timerlat...

Filters

Boot time tracing

histigrams


=======================================

基本功能

启动Trace
echo 1 > /proc/sys/kernel/ftrace_enabled
echo 1 > tracing_on

获取Event列表
ls events

使能Event
echo sched:sched_switch >> set_event
echo 1 > events/sched/sched_switch/enable

使能Kprobe
echo 'r:myretprobe do_sys_open $retval' >> /sys/kernel/debug/tracing/kprobe_events
会在events/kporbes/创建出myretprobe目录节点
使能节点开始抓取：
echo 1 > events/kprobes/myretprobe/enable
kprobe设置的详细格式见参考：
https://www.kernel.org/doc/Documentation/trace/kprobetrace.txt

获取Trace
cat trace
cat trace_pipe# 实时获取
cat trace_pipe > test.txt# 保存到文件

清空Trace
echo > trace

设置Event的PID
echo <pid> > set_event_pid# 白名单
echo <pid> > set_event_notrace_pid# 黑名单

设置使能的CPU
cat tracing_cpumask
echo 0x01 > tracing_cpumask

Per CPU Buffer
cat per_cpu/cpu0/trace
cat per_cpu/cpu0/trace_pipe

设置buffer size
echo 1024 > buffer_size_kb
echo 1024 > per_cpu/cpu0/buffer_size_kb

设置时间戳时钟类型
cat trace_clock
echo uptime > trace_clock

设置Tracing Options
ls options
cat options/xxx
echo 1 > options/xxx

抓取最大调用栈
echo 1 > /proc/sys/kernel/stack_tracer_enabled
cat stack_trace

创建独立Ftrace Instance
mkdir instances/test

设置Tracer
cat available_tracer
echo <tracer> > available_tracer

Tracers

NOP Tracer
事件trace，事件触发则打一个点，

设置tracer
echo nop > current_tracer

抓取事件发生时的调用栈
echo 1 > options/stacktrace
之后每个事件打点下面，都会包括这个事件发生时的调用栈

Function Tracer
函数trace，通过kprobe实现，每个函数进出都会打点，trace所有函数执行过程

设置tracer
echo  function > current_tracer

获得函数列表
cat available_filter_functions

设置跟踪函数
echo <function> > set_ftrace_filter# 白名单
echo <function> > set_ftrace_notrace# 黑名单

设置追踪PID
echo <pid> > set_ftrace_pid# 白名单
echo <pid> > set_ftrace_notrace_pid# 黑名单

Function Graph Tracer
功能与fucntion tracer相同，通过图形树显示整个函数执行过程，及每个函数执行时间

设置tracer
echo function_graph > current_tracer

设置追踪深度
echo 3 > max_graph_depth

设置追踪起始函数（根函数）
echo <function> > set_graph_function# 白名单
echo <function> > set_graph_notrace# 黑名单
* 这个节点设置的是追踪的起始函数，在这些函数开始追踪

设置跟踪函数、设置追踪PID
与Function Tracer使用相同功能节点，这个节点过滤的是打点的所有函数