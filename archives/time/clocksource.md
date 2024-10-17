# clocksource

clocksource就是一个固定频率(freq)的time counter，用来为时间系统提供时间计数，并为时间信息接口提供服务（timekeeping）。clocksource不是timer，他不提供中断。

**clocksource devices**

clocksource的实现，底层既可以是硬件，也可以是软件。实际上他就提供一个read接口，读取counter计数，通过counter/freq，就计算出了相对时间，然后为时间系统提供时间计算和计数。

jiffies是一个默认的clocksource，由于jiffies有固定频率，所以jiffies的值就可以作为这个counter值，用来转换时间。ARM arch timer的system counter，也是一个clocksource，这里只用了system counter，并不是每个core的timer。

以arm arch timer为例，system counter部分被实现为了clocksource，提供时间计数，为timekeeping服务，最终为上层提供出了各类时间获取接口。而processor timer部分，则实现为clockevent device，提供timer中断，最终提供出了tick device，为调度和timer服务。

**clocksource rating**

clocksource的频率越大，这个相对时间的精度就越高。比如一个10MHz时钟源，他的精度(rating)就是100ns。clocksource结构中有一个rating字段，代表着该时钟源的精度范围：

-   1-- 99 (ns)：不适合于用作实际的时钟源，只用于启动过程或用于测试
- 100--199 (ns)：基本可用，可用作真实的时钟源，但不推荐
- 200--299 (ns)：精度较好，可用作真实的时钟源
- 300--399 (ns)：很好，精确的时钟源
- 400--499 (ns)：理想的时钟源，如有可能就必须选择它作为时钟源

**clocksource mult/shift**

clocksource所表示的时间，可以通过下面算式计算：

```
t = cycle / freq
```

由于不能使用除法，所以使用mult/shift来代替除法计算

```
t = (cycle * mult) >> shift;
```

所以，clocksource需要根据freq计算出一组合适的mult, shift替代值，保存到结构体中。

**clocksource watchdog**

clocksource提供一个watchdog线程，来监控clocksource的精度，当他发现clocksource精度有问题时，就会把他的rating设置为0，表示精度极差，然后重新选择一个新的clocksource。

**clocksource selection**

clocksource会向上层timekeeping提供服务，只会有一个精度最高的clocksource被选择，用来提供服务(cur_clocksource)。

系统启动初始化阶段，会先使用jiffies作为默认clocksource，启动过程中，如果有新的clocksource注册进来，则会调用`clocksource_select`，重新选择出高精度clocksource，然后调用`timekeeping_notify`通知timekeeping系统更新clocksource。

timekeeping系统使用的clocksource中，除了cur_clocksource，还会有一个suspend_clocksource，这个clocksource可以在系统下电后继续运行，专门用于记录睡眠时间，

## Fiels

```
- include/linux/clocksource.h
- /kernel/time/clocksource.c
```

## Variables

`clocksource_list`

全局clocksource注册链表，根据精度排序。

`cur_clocksource`

当前选中的clocksource

`suspend_clocksource`

用来记录suspend时间

## Functions

`clocksource_done_booting`

Boot阶段默认clocksource初始化，使用jiffies作为默认clocksource

`clocksource_default_clock`

返回jiffies clocksource

`clocksource_select`

重新计算选择高精度的clocksource

`clocksource_register_hz`

注册clocksource

`clocksource_register_khz`

注册clocksource

`__clocksource_register_scale`

注册clocksource，会重新排序，执行clocksource select，并notify timekeeping。

`__clocksource_update_freq_scale`

通过freq计算更新clocksource的mult/shift

`timekeeping_notify`

通知timekeeping使用新的clocksource

## Usage Test

`/sys/devices/system/clocksources/<clocksource>`

- current_clocksource：当前使用的clocksource
- available_clocksource：所有注册的clocksource
- unbind_clocksource：?

```
logs

Register a new clocksource
clocksource: arch_sys_counter: mask: 0x1ffffffffffffff max_cycles: 0x1cd42e208c, max_idle_ns: 881590405314 ns
（max_cycles：counter最大值，max_idle_ns：最大发生翻转的时间）

Switch to new clocksource:
clocksource: Switched to clocksource arch_sys_counter
```

## Reference

<https://www.cnblogs.com/arnoldlu/p/7078234.html>

<https://www.kernel.org/doc/Documentation/timers/timekeeping.txt>