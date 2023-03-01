# Per Entity Load Tracking (PELT)

\* 代码解读

load_sum:	ready + running 负载
util_sum:	running 负载
runnable_sum	ready 负载

step1：之前负载经过衰减
step2：本轮负载新增

```
(past) d1 [   d2   ] d3 (now)
           perioods

[period_contrib + d1 ] [ ... d2 ... ] [ d3 + remain ]
                |---------- delta ---------|
|--------------------delta ----------------|
                                      |----| <-delta => period_contrib
```

period_contrib, 上次的d3
load, runnable, running => if task in this state, need accumulate
这就需要在任何一次task status切换时，执行这个更新函数，因为这个status信息认为上一轮periods全部都在这个status下

三类统计：
entity
rq
不同类对应自己的状态判断，rq的load应该是？

在计算完sum后，会计算avg，avg是sum/periods，其中load_avg比较特殊，会叠加weight（权重，从nice值映射过来），会导致load_avg最大不会大于se->load.weight
（`___update_load_avg`，load，也就是weight，是xxx_avg的最大值）

entity的load，反应entity有多大（多吃CPU）
rq的load，反应rq有多大（task的总量）

这个load数据，会用作cpufreq、负载均衡判断的依据