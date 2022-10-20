# arch_timer

**Arch Timer Hardware**

ARM Arch Timer，也叫ARM Generic Timer，是ARM核中的Timer。

Arch Timer硬件上包括两部分，一个是system counter（最终实现为clocksource），一个是processor timer（最终实现为clockevents）

ARM在所有核的前端提供了一个System Counter，System Counter与SOC实现相关，需要在启动阶段对其进行初始化后才能工作。

System Counter的基础上，ARM为每个核提供了四个Timer，这些Timer由System Counter驱动，可以为各个Core产生PPI中断。

**Arch Timer Driver**

Arch Timer Driver实现的功能包括：

1、硬件初始化
2、提供system counter操作接口（注册clocksource）
3、提供processor timer操作接口（并注册clockevent device）

## Files

```
- /drivers/clocksource/arm_arch_timer.c
```

## Variables

`struct clock_event_device __percpu *arch_timer_evt;`

每个核一个的clock event device，arch timer的feature支持持oneshot模式。

## Functions

```
arch_timer_init
    
    # 从device tree拿到node节点
    of_find_matchine_node(arch_timer_of_match)

    # 从node节点读取clock-frequency
    of_property_read_u32

    # 从node节点读取并申请irq中断号
    irq_of_parse_and_map

    # 完成各种子系统注册
    arch_timer_register

        # 创建clock_event_device（per_cpu）
        alloc_percpu(struct clock_event_device)

        # 注册clocksource（时间子系统注册）
        clocksource_register_hz

        # 注册timecounter（时间子系统注册）
        timecounter_init

        # 注册中断回调函数，并使能中断（per_cpu）
        request_percpu_irq

        # 完成硬件配置
        __arch_timer_setup(clock_event_device)

            # 初始化clock_event_device
            clk = ...

            # 注册clock_event_device，tick_device（时间子系统注册）
            clockevents_config_and_register

            # 使能中断（per_cpu）
            enable_percpu_irq
```

```
timer_handler（中断处理函数）

    # 读写TIMER_CTRL寄存器
    arch_timer_reg_read/write

    # 执行clock_event_device的回调函数（时间子系统回调）
    evt->event_handler (tick_handle_periodic)
```