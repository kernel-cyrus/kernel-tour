
# kvm


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







pkvm
===============

https://blog.csdn.net/Linux_Everything/article/details/109881719
https://www.youtube.com/watch?v=edqJSzsDRxk
https://mirrors.edge.kernel.org/pub/linux/kernel/people/will/slides/kvmforum-2020-edited.pdf


kvm!
================
https://zhuanlan.zhihu.com/p/528599297

https://www.youtube.com/watch?v=cuXyGkZRUKU
https://www.youtube.com/watch?v=oF8ydWwEuBI

http://www.linux-kongress.org/2010/slides/KVM-Architecture-LK2010.pdf
https://developer.ibm.com/articles/cl-hypervisorcompare-kvm/
https://documentation.suse.com/sles/12-SP4/html/SLES-all/cha-kvm.html
https://www.linux-kvm.org/page/KVM_Forum_2008

https://developer.arm.com/documentation/102142/0100/Introduction-to-virtualization


全虚拟化 vs 半虚拟化
-----------
virtio - 半虚拟化



is_hyp_mode_available - is el2 inited

is_kernel_in_hyp_mode - is kernel in el2

3 kvm mode: vhe, nvhe(hyp), pkvm(protected nvhe)

kvm hyp mode - nvhe mode



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


KVM Forum
---------------------
https://www.youtube.com/@kvmforum6546