# cpuidle

cpu idle分为三层

**idle sched class**


**cpu idle driver**


**arm implementation**



## Files

```
- /kernel/sched/idle.c			# cpu idle sched class (entry)
- /drivers/cpuidle/cpuidle.c		# cpuidle device and driver registation
- /drivers/cpuidle/cpuidle-arm.c	# arm cpu idle driver
- /arch/arm64/kernel/idle.c		# arm wfi idle
- /arch/arm64/kernel/cpuidle.c		# arm low level idle (psci suspend)
```