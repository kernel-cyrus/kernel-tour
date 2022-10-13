# tickdevice

tick device基于clockevent device实现

**periodic vs nohz**

not trigger ticks when idle.（"|" is a tick）

- tick_handle_periodic

```
|    T1    |    T2    |    IDLE    |    IDLE    |    T3    |
```

- tick_nohz_handler（CONFIG_NOHZ_IDLE）

```
|    T1    |    T2    |          IDLE           |    T3    |
```

**high resolution mode**

allow timer event inside a tick, which means you can fire a timer event anytime.（"^" is a timer event）

- tick_nohz_handler

```
|    T1    |    T2    |^         IDLE           |^   T3    |
```

- hrtimer_interrupt

```
|    T1    |   T ^ 2  |^         IDLE     ^  ^  |^   T3    |
```

## Reference

<https://www.youtube.com/watch?v=Puv4mW55bF8&ab_channel=TheLinuxFoundation>