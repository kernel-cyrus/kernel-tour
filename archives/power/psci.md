# psci

PSCI是在ARMv8后提出的新的电源管理框架。主要的工作是将原来在Kernel中完成的电源操作，通过SMC Call的方式，下移到ATF中。

使用PSCI，需要在DTS中配置PSCI节点，并指定PSCI的版本（ATF支持的PSCI操作的版本），PSCI的方法（HVC/SMC）。

之后PSCI框架会在系统初始化时，根据DTS的配置，完成CPU PSCI操作的绑定。

绑定后，对CPU进行电源管理操作，会通过SMC + Func ID的方式，传递给ATF完成。

真正的电源操作，在ATF中完成。

PSCI的电源管理框架的层次结构如下图所示：

```
[	   SMP Framework	]
[	      cpu_ops		]
[	    cpu_psci_ops	]
[	      psci_ops		]
[	      HVC/SMC		]
---------------------------------
[	     ATF or HYP		]
```

其中，SMP的框架调用cpu_ops来完成单个CPU的电源操作。

cpu_ops（cpu_ops.c）在初始化时，会从dts中读取cpu节点的enable-method，绑定对应的cpu_ops实现（psci或spin table），这里我们只关心基于psci实现的cpu_ops（cpu_psci_ops）

cpu_psci_ops使用psci_ops实现。

psci_ops在初始化时，根据dts描述，绑定好对应的底层操作函数。

这些底层函数是基于HVC或SMC + Func ID的简单封装。

## Files

```
- /arch/arm64/kernel/smp.c		# SMP Operations, based on cpu_ops framework
-------------------------------------
- /arch/arm64/kernel/cpu_ops.c		# CPU Oerations (cpu_ops) framework, point to cpu_psci_ops
- /arch/arm64/kernel/psci.c		# PSCI cpu_ops implementation (cpu_psci_ops), based on psci_ops
- /arch/arm64/include/asm/cpu_ops.h
-------------------------------------
- /drivers/firmware/psci/psci.c		# PSCI SMC/HVC Wrap Functions (psci_ops)
- /include/linux/psci.h
- /include/uapi/linux/psci.h		# PSCI Function IDs
```

## Structures

```
struct psci_operations {
	u32 (*get_version)(void);
	int (*cpu_suspend)(u32 state, unsigned long entry_point);
	int (*cpu_off)(u32 state);
	int (*cpu_on)(unsigned long cpuid, unsigned long entry_point);
	int (*migrate)(unsigned long cpuid);
	int (*affinity_info)(unsigned long target_affinity,
			unsigned long lowest_affinity_level);
	int (*migrate_info_type)(void);
};
```

## Variables

`struct psci_operations psci_ops`

全局psci_ops变量

`invoke_psci_fn`

psci method function，hvc / smc

## DTS

```
psci {
	compatible = "arm,psci-1.0";	// 配置psci版本
	method = "smc";			// 配置psci方法
};

&cpu0 {
	enable-method = "psci";		// 使用psci控制cpu上下电(cpu_ops use psci)
};
```

## Functions

```
> psci_dt_init

在setup_arch中被初始化

会读取dts中psci节点，完成对应版本的psci和method（invoke_psci_fn）的初始化。

	> get_set_conduit_method

	如果是smc，invoke_psci_fn=\_\_invoke_psci_fn_smc，底层使用arm_smccc_smc发送指令

	如果是hvc，invoke_psci_fn=\_\_invoke_psci_fn_hvc，底层使用arm_smccc_hvc发送指令

	指令对应的Function ID在psci.h中定义，底层smc、hvc接口在smccc-call.S中实现

	> psci_probe

	初始化psci_ops全局变量，绑定的所有回调函数都是对smc + func_id的封装。

```

`psci_features`

通过psci call(smc)请求检查某个Func ID是否支持。


`psci_ops`

Handle CPU PM Ops

```
.get_version = psci_0_2_get_version,
.cpu_suspend = psci_0_2_cpu_suspend,
.cpu_off = psci_0_2_cpu_off,
.cpu_on = psci_0_2_cpu_on,
.migrate = psci_0_2_migrate,
.affinity_info = psci_affinity_info,
.migrate_info_type = psci_migrate_info_type,
```

`psci_sys_reset_nb`

Handle System Reset

`psci_sys_poweroff`

Handle System Power Off