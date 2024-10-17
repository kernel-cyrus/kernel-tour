# eas

当一个task被唤醒时，可以选择放到本CPU，也可以选择放到其他CPU。上核后又可能引起升频。

EAS通过建立util与energy的关系，从而可以提前评估出这几种决定对energy的影响，最终找到最优解。

## CPU Capacity

cpu的capacity，一般是通过1024归一化的dmips。

可以通过以下节点，获知各个CPU的Capacity

`/sys/devices/system/cpu/cpu*/cpu_capacity`

cpu内部频点的capacity，简单通过频率比通过线性计算得出。

EM提供了文件节点，来查看各个频点对应的功耗

`/sys/devices/system/cpu/cpu*/cpu_capacity`

capacity，在调度侧，就是一个task的util。

## OPP Table

OPP Table就是保存在dts中的一个freq table，保存了频率和对应电压电压信息。

CPU topology信息中，保存了cpu的capacity的信息。

Files:

```
- /drivers/opp/of.c
- /drivers/cpufreq/scmi-cpufreq.c
```

Reference: /Documentation/power/opp.rst

## Energy Model

EM框架管理着系统中每个“性能域”的功率成本表，并提供了一个em_cpu_energy接口，可以获得当前util对应的energy信息。从而建立起util->energy的评估关系。

Files:

```
- /kernel/power/energy_model.c		# energy model
- /include/linux/energy_model.h		# energy model
```

`struct em_perf_domain`

频点/功耗信息表。

可以理解为一个power domain，一组共用一个调频表的CPU合集。每一组频点/功耗信息，保存在一个em_perf_state中。

`em_dev_register_perf_domain`

通过cb，向device中创建一个em_perf_domain。

会获取完整OPP表，构造出em_perf_domain（里面保存了所有em_perf_state），保存到dev->em_pd中。

`em_cpu_get` / `em_pd_get`

通过CPU ID或者CPU Device指针，获得对应的em_perf_domain

`em_cpu_energy`

通过传入cpu_util，计算power domain消耗的energy（后面用于预测task上核后，util发生变化，对energy的影响）

其中，对各个变量的解读：

```
ps->cap = 某个频点对应的归一化算例，也就是freq normalized capacity
cpu_nrg = 叠加占空比的实际cap使用
        = (cpu_util / ps->cap) * ps->power（当前频点功耗）
        = ps->cost * (cpu_util / scale_cpu)（当前频点下的Normalized Power）
         （提前计算的）
ps->cost = ps->power * (cpu_max_freq / ps->freq)当前频点下在当前CPU的Normalized Power
```

Reference: /Documentation/power/energy-model.rst

## EAS

EAS建立在Energy Model基础上，在EM提供了归一化的util及对应energy换算关系后，EAS就可以通过这个换算关系计算出一个固定util的task，在放置到不同核上时，得到的最终频点及util，以及对应的energy信息。从而决定上核的选择。

EAS的核心代码，主要修改了Task Wakeup的流程

```
try_to_wakeup
-> select_task_rq_fair
-> find_energy_efficient_cpu
-> compute_energy
-> em_cpu_energy
```

选核时，为了避免迁核带来的性能损失，只有当功耗收益大于6%时，才会真正触发迁核。

另外，task有mis-fit标记，当需要强制一个task迁核时，可以设置其mis-fit标记。

Reference: Documentation/scheduler/sched-energy.rst

## Load Balance

EAS只在中低负载场景下生效，当CPU平均负载大于80%时，整个域会被标记为overutilized状态，禁用EAS，切换为SMP负载均衡算法。

## Schedutil

schedutil就是通过归一化的util，直接推算出目标频点

util = cpu_load * cpu_scale * freq_scale（也就是CPU占空比 * 频点归一化算力）

任何时候，我们都可以得到这个normalized util（u_cfs + u_rt + u_irq + udl，见 `effective_cpu_util`。

目标频点：next_freq = (util * 1.25) * max_freq，见 `get_next_freq`

**uclamp**

clamp会消掉util，从而影响频率，将频率限定在某个区间内。

也就是不论task运行有多繁忙，最终计算出的util都会被限制在一个区间内。

uclamp在cfs、rt的rq上生效，也就是从rq读取util时，已经是经过uclamp过滤的了。

## Reference

https://mp.weixin.qq.com/s/HgEJ_IO-Gcy66vxMdsIGkQ