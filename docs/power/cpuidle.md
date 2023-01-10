# cpuidle


## Files

```
- /drivers/cpuidle/cpuidle.c		# cpuidle device and driver registation
- /drivers/cpuidle/cpuidle-arm.c	# arm cpu idle driver
- /kernel/sched/idle.c			# cpu idle sched class (entry)
- /arch/arm64/kernel/idle.c		# arm wfi idle
- /arch/arm64/kernel/cpuidle.c		# arm low level idle (psci suspend)
```