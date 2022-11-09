# delay

ndelay，死等（纳秒），这个接口不一定在所有平台都支持

udelay，死等（微秒），推荐使用

mdelay，死等（毫秒），一般不推荐使用，时间太久

\* delay在原子操作及非原子操作均可以使用，如在spinlock的critical section中。

# sleep

usleep_range，睡眠等，会创建hrtimer等待唤醒，精度高，并且指定range后timer可以被合并触发，减少中断次数，推荐使用

msleep，睡眠等，会创建legacy timer等待唤醒

msleep_interruptible，睡眠等，会创建legacy timer等待唤醒

ssleep，睡眠等，就是msleep的封装

fsleep，睡眠等，推荐使用！会根据sleep时间，自动帮你选择合适的sleep函数

\* sleep不适合原子操作，不能在critical section中使用。

\* usleep不存在，没那么高的精度

\* msleep只适用于10ms+，10ms以内，精度不够

# files

`/include/linux/delay.h`

# reference

https://docs.kernel.org/timers/timers-howto.html