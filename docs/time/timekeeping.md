# timekeeping

timekeeping系统为linux提供各类时间及获取函数。

timekeeping从persistent clock(rtc)获得初始时间，然后绑定clocksource，后面时间获取将通过clocksource + 各类时间的offset完成。

## Files

```
- /kernel/time/timekeeping.c			# Time interface implementation
- /include/linux/timekeeper_internal.h
- /include/linux/timekeeping.h			# Time interface declaration
- /include/linux/ktime.h			# ktime_t convertion
```

## Interfaces

### Time Clocks

Timekeeping系统提供了多种clock，每种clock有对应接口来获得各类时间，以下接口均返回ktime_t纳秒时间。

除了纳秒，ktime可以转换成各种时间格式，接口在ktime.h中定义

**CLOCK_MONOTONIC**

`ktime_get`

系统启动时间，系统睡眠停止计时

**CLOCK_BOOTTIME**

`ktime_get_boottime`

系统启动时间，系统睡眠不停止计时

**CLOCK_REALTIME**

`ktime_get_real`

Unix时间戳，不应该用于interval计算，可能被NTP更新。

**CLOCK_TAI（不常用）**

`ktime_get_clocktai` 

Like ktime_get_real(), but uses the International Atomic Time (TAI)
reference instead of UTC to avoid jumping on leap second updates.
This is rarely useful in the kernel.

**CLOCK_MONOTONIC_RAW（不常用）**

`ktime_get_raw`

Like ktime_get(), but runs at the same rate as the hardware
clocksource without (NTP) adjustments for clock drift. This is
also rarely needed in the kernel.

### Return Formats

上述所有clock，处理可以返回ktime_t，也有相应接口返回其他time数据类型

**返回u64（ns）**

```
u64 ktime_get_ns(void)
u64 ktime_get_boottime_ns(void)
u64 ktime_get_real_ns(void)
u64 ktime_get_clocktai_ns(void)
u64 ktime_get_raw_ns(void)
```

**返回timespec64（sec + ns）**

```
void ktime_get_ts64(struct timespec64*)
void ktime_get_boottime_ts64(struct timespec64*)
void ktime_get_real_ts64(struct timespec64*)
void ktime_get_clocktai_ts64(struct timespec64*)
void ktime_get_raw_ts64(struct timespec64*)
```

**返回time64_t（s）**

```
time64_t ktime_get_seconds(void)
time64_t ktime_get_boottime_seconds(void)
time64_t ktime_get_real_seconds(void)
time64_t ktime_get_clocktai_seconds(void)
time64_t ktime_get_raw_seconds(void)
```

### Coarse Interface

系统中除了上述Clock，还提供了两个粗粒度Clock Type：

- CLOCK_MONOTONIC_COARSE
- CLOCK_REALTIME_COARSE

这两个Clcok速度快，精准度略低。同时也提供了相应的获取接口，类型与其他接口也保持一致。

这些时间获取接口，跳过了硬件读取过程，会节省100cycle时间，可以用于fast路径。同时会有一个tick时间的误差（250Hz，4ms）。

```
ktime_t ktime_get_coarse(void) 
ktime_t ktime_get_coarse_boottime(void) 
ktime_t ktime_get_coarse_real(void) 
ktime_t ktime_get_coarse_clocktai(void) 
```

```
u64 ktime_get_coarse_ns(void) 
u64 ktime_get_coarse_boottime_ns(void) 
u64 ktime_get_coarse_real_ns(void) 
u64 ktime_get_coarse_clocktai_ns(void) 
```

```
void ktime_get_coarse_ts64(struct timespec64*) 
void ktime_get_coarse_boottime_ts64(struct timespec64*) 
void ktime_get_coarse_real_ts64(struct timespec64*) 
void ktime_get_coarse_clocktai_ts64(struct timespec64*) 
```

### Fast Interface

与Coarse类似，但是可以用于NMI中...

```
u64 ktime_get_mono_fast_ns(void)
u64 ktime_get_raw_fast_ns(void)
u64 ktime_get_boot_fast_ns(void)
u64 ktime_get_tai_fast_ns(void)
u64 ktime_get_real_fast_ns(void)
```

### Deprecated & Replacement

下面接口已经废弃使用，需要被替换：

> void ktime_get_ts(struct timespec\*)

Use `ktime_get()` or `ktime_get_ts64()` instead.

> void do_gettimeofday(struct timeval\*)<br>
> void getnstimeofday(struct timespec\*)<br>
> void getnstimeofday64(struct timespec64\*)<br>
> void ktime_get_real_ts(struct timespec\*)<br>

`ktime_get_real_ts64()` is a direct replacement, but consider using monotonic time (ktime_get_ts64()) and/or a ktime_t based interface (ktime_get()/ktime_get_real()).

> struct timespec current_kernel_time(void)<br>
> struct timespec64 current_kernel_time64(void)<br>
> struct timespec get_monotonic_coarse(void)<br>
> struct timespec64 get_monotonic_coarse64(void)<br>

These are replaced by `ktime_get_coarse_real_ts64()` and `ktime_get_coarse_ts64()`. However, A lot of code that wants coarse-grained times can use the simple ‘jiffies’ instead, while some drivers may actually want the higher resolution accessors these days.

> struct timespec getrawmonotonic(void)<br>
> struct timespec64 getrawmonotonic64(void)<br>
> struct timespec timekeeping_clocktai(void)<br>
> struct timespec64 timekeeping_clocktai64(void)<br>
> struct timespec get_monotonic_boottime(void)<br>
> struct timespec64 get_monotonic_boottime64(void)<br>

These are replaced by `ktime_get_raw()` / `ktime_get_raw_ts64()`, `ktime_get_clocktai()` / `ktime_get_clocktai_ts64()` as well as `ktime_get_boottime()` / `ktime_get_boottime_ts64()`. However, if the particular choice of clock source is not important for the user, consider converting to `ktime_get()` / `ktime_get_ts64()` instead for consistency.

## Structures

所有time相关数据，保存在 `tk_core` 中（timekeeper）

```
struct timekeeper {
	struct tk_read_base	tkr_mono;
		xtime_nsec				// all clock offset (count by clocksource)
							// the value is shifted, should >> shift
		base					// CLOCK_MONOTONIC base
		base_real				// CLOCK_REALTIME base
	u64			xtime_sec;		// CLOCK_REALTIME (s)
	unsigned long		ktime_sec;		// CLOCK_MONOTONIC (s)
	struct timespec64	wall_to_monotonic;	// CLOCK_REALTIME + this = CLOCK_MONOTONIC
	struct timespec64	monotonic_to_boot;	// CLOCK_MONOTONIC + this = CLOCK_BOOTTIME
	ktime_t			offs_real;		// CLOCK_MONOTONIC + this = CLOCK_REALTIME
	ktime_t			offs_boot;		// CLOCK_MONOTONIC + this = CLOCK_BOOTTIME
};
```

## Functions

### Timekeep init

`timekeeping_init`

1. 读取persistent time（rtc），并初始化boot offset，后面再获取时间时就不再从rtc读取。
2. 通过clocksource_default_clock获取并初始化timekeeping使用的clocksource
3. 初始化tk_core

### Timekeep suspend / resume

timekeeping在syscore中注册了suspend/resume接口，用于保存suspend/resume时间。

suspend时，会通过rtc，以及clocksource的suspend clocksource来记录睡眠和唤醒时间，更新到tk_core的sleep time相关数据中。

`timekeeping_init_ops`

向syscore注册suspend、resume接口

`timekeeping_suspend`

分别获取睡眠时的persistent clock、clocksource counter，保存到timekeeping_suspend_time、suspend_start。

`timekeeping_resume`

再次获取唤醒时的persistent clock、clocksource counter，计算出睡眠时间，更新到tk_core的sleep相关数据中。

`read_persistent_clock64`

获取persistent clock值

`clocksource_start_suspend_timing`

记录clocksource counter到suspend_start变量

`clocksource_stop_suspend_timing`

获取suspend_start，并计算出睡眠sec返回

`__timekeeping_inject_sleeptime`

更新tk_core中的sleep相关成员。

### Timekeep update

细粒度接口的time数据，会从clocksource读取计算，粗粒度接口time数据会直接从tk_core中取得。tk_core的数据会在每个tick中得到更新。

`update_wall_time`

每个tick触发，从clocksource重新获取数据，并更新tk_core到最新。

`tick_do_update_jiffies64`

tick回调函数，更新jiffies，update tk_core到最新时间。

## Reference

<https://docs.kernel.org/core-api/timekeeping.html>

<https://www.cnblogs.com/arnoldlu/p/7078241.html>

<https://www.cnblogs.com/arnoldlu/p/7078250.html>