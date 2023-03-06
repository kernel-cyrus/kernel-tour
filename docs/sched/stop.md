# stop sched class

**Stop Class**

stop调度类是优先级最高的调度类，允许以最高优先级运行一个stop task，这个task不能会被抢占，会一直运行到task结束或者主动让出CPU。

**Stop Machine**

目前stop task只有stop machine机制在使用（实现了一个stop task）。

stop machine机制用于启动一个stop task，这个task会终止掉所有调度行为，直到运行结束。

stop machine提供了两个创建stop task的接口：

`stop_machine`

将所有核的调度停止，

`stop_cpus`

queue_stop_cpus_work
	&stopper->works

stop multiple cpus, Execute @fn(@arg) on online cpus in @cpumask


`stop_one_cpu_nowait`


一般来说, 内核会在如下情况下使用stop_machine技术
module install and remove，cpu hotplug，memory hotplug，ftrace，hwlat_detector，Kernel Hotpatch
stop machine在系统启动时，会在每个cpu上创建一个smp_hotplug_thread，这些线程最终会对应为kthread，名字是migration/cpuid。这些kthread会被smp_hotplug_thread.create回调在创建时添加到STOP调度器rq中。这些kthread的统一线程函数是kthreadd - smpboot_thread - cpu_stopper_thread（套了3层warpper），最终，线程函数实现了一个looper，来处理work list中的work，没有work则休眠。
stop_one_cpu或stop_machine会将待执行函数加入到work list中，然后唤醒kthread，由于kthread是在STOP中，实现了最高优先级执行，也就是停止所有CPU工作，执行用户函数的功能。

smp hotplug thread
smpboot.h / smpboot.c
smp_hotpplug_thread是支持hotplug的kthread，他是在kthreadd线程warpper函数外，再套了一层smpboot_thread warpper，在kthread原有功能外，实现了自定义park/unpark的功能。在cpu offline和online时，除了调用kthread正常park/unpark外，自定义的park/unpark函数也会被调用。除了park/unpark外，还有一些其他的自定义回调函数，可以查看smpboot.h中这些接口的说明。
kthread所对应的task struct，会储存到.store中。



stop中目前只有migration kthread，用来执行stop_one_cpu或stop_machine指定的work。


Stop Machine
