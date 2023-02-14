# cpufreq

## Driver

## Policy

`/sys/devices/system/cpu/cpu0/cpufreq/policyX`

```
scaling_driver			r	# cpufreq driver name
affected_cpus			r	# cpu
related_cpus			r	# <?>
scaling_cur_freq		rw	# cur freq
scaling_min_freq		rw	# min freq limit
scaling_max_freq		rw	# max freq limit
scaling_governor		rw	# selected governor
scaling_available_governors	r	# available governors
scaling_setspeed		r	# <not supported>
cpuinfo_min_freq		r	# hardware min freq
cpuinfo_max_freq		r	# hardware max freq
cpuinfo_transition_latency	r	# hardware latency
```

## Governors

**powersave**

always lowest frequency

**performance**

always highest frequency

**schedutil**

uses CPU utilization data from scheduler to determine target frequence

**userspace**

allow user space set the frequency

**ondemand**

uses CPU load as a CPU frequency selection metric

it reaches the target by CPU load, 0->min, 100->max

**conservative**

uses CPU load as a CPU frequency selection metric

it reaches the target step by step, up_threshold->up, down_threshold->down

## Reference

<https://www.kernel.org/doc/html/latest/admin-guide/pm/cpufreq.html>