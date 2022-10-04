# arch_timer

arch timer大致包括两部分内容，一部分是timer硬件的初始化，中断注册，另一部分是时间子系统的注册（clock_event_device，clock_source，time_counter）。

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
        arch_timer_setup(clock_event_device)

            # 初始化clock_event_device
            clk = ...

            # 注册clock_event_device（时间子系统注册）
            clockevents_config_and_register

            # 使能中断（per_cpu）
            enable_percpu_irq


timer_handler（中断处理函数）

    # 读写TIMER_CTRL寄存器
    arch_timer_reg_read/write

    # 执行clock_event_device的回调函数（时间子系统回调）
    evt->event_handler (tick_handle_periodic)
```