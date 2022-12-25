
KVM framework


ARM Exception Levels


KVM on ARM


Create VM with KVM


nVHE vs VHE
	init flow and el switch


Google PKVM












===============================================


# kvm


kvm-arm.mode =
	protected: 	KVM_MODE_PROTECTED 	- kernel in el1, protected kvm in el2 (pkvm or protected nvhe mode)
	nvhe（默认）: 	KVM_MODE_DEFAULT 	- kernel in el1, default kvm in el2 (nvhe or hyp mode)
						  kernel in el2, kvm is within kernel in el2(vhe mode)
	none: 		KVM_MODE_NONE 		- kernel in el1 or el2, kvm is disabled(kvm disabled)
Kernel Boot EL + kvm-arm.mode共同决定最终Kernel EL
如果kvm-arm.mode对EL无要求（nvhe），则Kernel EL为Boot EL
如果kvm-arm.mode对Kernel EL有要求（protected），则Kernel会切换EL，为Hyp让出EL。


编译时，vhe hyp和nvhe hyp都编译进来了（Makefile中定义的）

EL切换流程
-------------------
Bootwrapper会初始化EL1运行环境（相关寄存器），如果使能了PSCI，则会允许陷入EL3，并设置EL3异常像量表（VBAR_EL3，0x8d000800）来响应PSCI的SMC请求。最后初始化SCTRL_EL1，SCTRL_EL2后，陷入到Kernel。

kernel entry
-------------------
进入Kenrel Entry（0x8d200000），会init_kernel_el。

如果是EL2进入，则会init_el2，初始化EL2的运行环境及异常像量表（VBAR_EL2，0x8d683000），并禁用VHE（在后面hyp init时重新切换），然后进入EL1开始运行Kernel。

如果是EL1进入，则直接init_el1，然后继续执行。不论进入是EL几，都会完成到EL1的切换。

进入Kernel的EL级别（BOOT_CPU_MODE）会保存起来，供后面VHE流程判断使用。BOOT_CPU_MODE的值很有趣，E11、E12分别表示EL1、EL2。这个变量，就是后面CPU那行"All CPU(s) boot at EL2"的内容。

switch vhe
----------------------
start_kernel前，primary switch会执行switch_to_vhe，使kernel进入EL2

hyp_mode_check
----------------------
打印__boot_cpu_mode

pkvm会把kernel切回el1？而default模式则不会？？看上去是这样！
（vhe hyp和nvhe hyp分别是运行在el2和el1的hyp？？）

hyp init
选择是否进入VHE / nVHE模式


-------------------------------------------------

vhe的hyp，nvhe的hyp不一样，是两个hyp实现。
以前的nvhe模式下，kernel在el1，hyp在el2，vm在el1，每次切换需要经过hyp切换
vhe允许kernel运行在el2，hyp也在el2，vm在el1，大幅提高了性能。？

to el2 and beyond, Linaro
https://www.youtube.com/watch?v=j0bp4fnO98w&ab_channel=KVMForum
http://events17.linuxfoundation.org/sites/events/files/slides/To%20EL2%20and%20Beyond_0.pdf


pkvm
===============

lwn:https://blog.csdn.net/Linux_Everything/article/details/109881719
youtube:https://www.youtube.com/watch?v=edqJSzsDRxk&ab_channel=KVMForum
slides:https://mirrors.edge.kernel.org/pub/linux/kernel/people/will/slides/kvmforum-2020-edited.pdf


kvm!
================
https://zhuanlan.zhihu.com/p/528599297

https://www.youtube.com/watch?v=cuXyGkZRUKU&t=154s&ab_channel=KVMForum
https://www.youtube.com/watch?v=oF8ydWwEuBI&ab_channel=DREAMS%28DistributedREal-TimeArchitectureforMixedCriticalitySystems%29
https://www.youtube.com/watch?v=jZNXtqFJpuc&ab_channel=GlobalLogicUkraine


```
module_init(arm_init)
	kvm_init
		kvm_arch_init


cpuhp callbacks:
	kvm_starting_cpu -> hardware enable
	kvm_dying_cpu -> hardware disable

reboot callback:
	kvm_reboot_notifier -> kvm_reboot

kvm_syscore_ops
	suspend, resume callback

kvm_preempt_ops
	schedule in
	schedule out

kvm_vcpu_cache - kvm_vcpu struct pool
cpu_kick_mask - per cpu ?kick mask?

kvm_dev - kvm misc device, create the kvm fs node, with ioctl support
```

kvm ioctl(kvm_chardev_ops, kvm_dev_ioctl)
	create vm: kvm_dev_ioctl_create_vm, return <vm fd>
	or pass through ioctl -> kvm_arch_dev_ioctl

```
kvm_dev_ioctl_create_vm
	kvm_create_vm

	return vm fd with kvm_vm_fops -> kvm_vm_ioctl
```

vm ioctl(kvm_vm_fops, kvm_vm_ioctl)
	KVM_CREATE_VCPU
	KVM_SET_USER_MEMORY_REGION
	KVM_IRQFD
	KVM_IOEVENTFD
	KVM_CREATE_DEVICE


kvm device ioctl(kvm_device_fops, kvm_device_ioctl)
	KVM_SET_DEVICE_ATTR
	KVM_GET_DEVICE_ATTR
	KVM_HAS_DEVICE_ATTR
	pass through ioctl -> dev->ops->ioctl


vcpu ioctl(kvm_vcpu_fops, kvm_vcpu_ioctl)
	KVM_RUN
	KVM_GET_REGS
	KVM_SET_REGS


vm fault: kvm_vcpu_fault

kvm_mmu_notifier_ops
	invalidate_range
	invalidate_range_start
	invalidate_range_end
	clear_flush_young
	clear_young
	test_young
	change_pte
	release

lots of kvm operations called from arm
	kvm_vcpu_halt

memragion and memslot

io bus