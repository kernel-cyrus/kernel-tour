# cpu topology

CPU结构在硬件层次上可以划分为：Socket，Cluster，Core，CPU四层：

```
arch									topology	dts		domain
--------------------------------------------------------------------------------------------------------------
Cpu	| CPU0 | CPU1 | CPU2 | CPU3 | CPU4 | CPU5 | CPU6 | CPU7 |	->core_id	"cpu"
--------------------------------------------------------------------------------------------------------------
Node	|                         NODE-0                        |	->package_id	“socket”	(DIE)
Cluster |         CLUSTER-0         |         CLUSTER-0         |	->cluster_id	“cluster”	(MC)
Core    |    CORE-0   |    CORE-1   |    CORE-2   |    CORE-3   |	->thread_id	“thread”	(SMT)
--------------------------------------------------------------------------------------------------------------
```

在正常的ARM SOC上，不存在多路（只有一个Node），也不存在SMT超线程，所以只会有一个Node和CPU、Cluster、Node三个层次。

**CPU Topology 初始化**

CPU的拓扑结构在DTS的`cpu-map`中描述，启动时通过 `init_cpu_topology` 完成初始化：

```
#0  parse_dt_topology () 		at drivers/base/arch_topology.c:619
#1  init_cpu_topology ()		at drivers/base/arch_topology.c:791
#2  smp_prepare_cpus (max_cpus=256) 	at arch/arm64/kernel/smp.c:726
#3  kernel_init_freeable () 		at init/main.c:1592
#4  kernel_init () 			at init/main.c:1499
#5  ret_from_fork () 			at arch/arm64/kernel/entry.S:867
```
内核用 `cpu_topology[NR_CPUS]` 储存每个CPU的Topology：

```
struct cpu_topology {
	int thread_id;			// -1
	int core_id;			// mpidr_el1寄存器中获取
	int cluster_id;			// mpidr_el1寄存器中获取
	int package_id;			// mpidr_el1寄存器中获取
	int llc_id;
	cpumask_t thread_sibling;	// dts: "cpu-map"，thread中包含的cpu
	cpumask_t core_sibling;		// dts: "cpu-map"，core中包含的cpu
	cpumask_t cluster_sibling;	// dts: "cpu-map"，cluster中包含的cpu
	cpumask_t llc_sibling;		// dts: "cpu-map"
};
```

其中，id为硬件定义的ID，启动时通过 `store_cpu_topology()` 从寄存器中读取。

**Sched Domain 初始化**

调度域（sched_domain）的拓扑结构基于CPU拓扑结构，如开头的CPU Topology图所示，系统有三层调度域：DIE、MC、SMT。

每一级调度域下，都挂了一组调度组（sched_group），sched_group是一组CPU，根据调度域层次不同，可以是一个Cluster的CPU组或者一个独立CPU。

一个CPU在做负载均衡的时候，实际就是在其最近的一个调度域中，以调度组为粒度，做调度组间的均衡。

下图展示了sched_domain, sched_group及cpu topology间的关系：

```
DIE	|                  sched_domain(DIE)                   |
	|       sched_group        |       sched_group         |

MC	|     sched_domain(MC)     |      sched_domain(MC)     |
	|sched_group | sched_group | sched_group | sched_group |

CPU	|    CPU0    |     CPU1    |    CPU2     |    CPU3     |
```

以上图为例，CPU0挂在两个调度域中：domain0（MC），domain1（DIE）（见 `/sys/kernel/debug/sched/domains`），MC域的sched_group中包括一个独立CPU，而DIE域的sched_group中包括一组CPU（一个cluster）。

topology在`build_sched_domains`时被初始化，保存在`sched_domain_topology`中：

```
#0  build_sched_domain () 	at kernel/sched/topology.c:70
#1  build_sched_domains () 	at kernel/sched/topology.c:70
#2  sched_init_domains () 	at kernel/sched/topology.c:2462
#3  sched_init_smp () 		at kernel/sched/core.c:9485
#4  kernel_init_freeable () 	at init/main.c:1603
#5  kernel_init () 		at init/main.c:1499
#6  ret_from_fork () 		at arch/arm64/kernel/entry.S:867

sched_domain_topology[]:
	SMT(cpu_smt_mask)
		->sched_domain
		->sched_group
	MC(cpu_coregroup_mask)
		->sched_domain
		->sched_group
	DIE(cpu_cpu_mask)
		->sched_domain
		->sched_group
	NODE(cpu_cpu_mask)
		->sched_domain
		->sched_group
}
```

初始化后，`sched_domain_topology`无法从GDB获取？

每个CPU所属的sched_domain及对应的sched_group，保存在自己CPU的`runqueues->sd`中：

```
runqueues (CPU0)
	-> sd (sched_domain)
		-> name: "MC"
		-> parent: --> (DIE)
		-> child:  --> (NONE)
		-> groups (sched_group)
			-> cpumask: this cpu id
			-> ref.counter: numbers of cpus in this domain (MC)
			-> next: siblings cpus of in this domain (MC)
```

## sysfs

`/sys/kernel/debug/sched/domains`

每个CPU对应的调度域

`/sys/devices/system/cpu/<cpu>/topology`

```
- cluster_cpus:01		cpu id
- cluster_cpus_list:0		cpu id
- cluster_id:-1			-1
- core_cpus:01			cpu id
- core_cpus_list:0
- core_id:0			cluster内部的cpu id（每个cluster从0开始计数）
- core_siblings:0f		cluster内部cpumask
- core_siblings_list:0-3	cluster内部cpu id
- package_cpus:0f		cluster内部cpumask
- package_cpus_list:0-3		cluster内部cpu id
- physical_package_id:0		cluster id
- thread_siblings:01
- thread_siblings_list:0
```

每个CPU在cpu-map中的描述信息

## Files

```
- /bindings/cpu/cpu-topology.txt	# cpu dts docs
- /drivers/base/arch_topology.c		# cpu dts parse
- /drivers/base/topology.c		# topology sysfs nodes
- /arch/arm64/kernel/topology.c		# arch cpuid
- /kernel/sched/topology.c		# sched domains
```

## Reference

<https://blog.csdn.net/m0_74282605/article/details/127965951>