# kvm

（只是大致原理，待详细研究整理：ARM Virtualization、KVM Implementation）

KVM是Kernel Virtual Machine，内核中实现的虚拟机框架。

KVM实现了对VM、VCPU、VMEM的创建和管理，并对Userspace提供了通用的操作节点和ioctl协议。

```
Type-1模式下(XEN):			Type-2模式下(KVM)：
EL0: [ APP  APP ] [ APP  APP ]		EL0: [    APP  APP    ] [ APP  APP ]
EL1: [ Guest OS ] [ Guest OS ]		EL1: [ Host OS + KVM  ] [ Guest OS ]
EL2: [        XEN Hyp        ]		EL2: [           KVM Hyp           ]
```

不同于XEN这类Type-1 Virtualization，Type-2方式的KVM内嵌在Kernel里，这使得KVM可以充分利用Kernel提供出来的功能，而Hyp的功能则非常简单，只提供World Switch功能。

X86和ARM平台都有自己的虚拟化框架，思想和实现都截然不同。KVM在内核中封装了对虚拟机管理和操作的通用功能，而底层实现则根据不同硬件平台，由各家自己来实现。

**ARM Virtualization**

```
EL0: [    VMM  APP    ] [ APP  APP ]
EL1: [ Host OS + KVM  ] [ Guest OS ]
EL2: [           KVM Hyp           ]
```

在ARM平台上（以nVHE模式为例），虚拟化使用Exception Model来实现。其中，VMM为用户态的虚拟机管理器，用于创建和操作虚拟机。EL1的KVM为虚拟机管理框架，对上层提供文件节点及ioctl接口。EL1的KVM框架，在接受到对虚拟机操作的ioctl后，通过调用Kernel功能，完成各种创建操作。KVM Hyp用于做虚拟机之间的World Switch，这种Switch非常像Process的Context Switch，只不过这里是Guest OS间的Context Switch。

在虚拟机侧，ARM为Guest OS提供了独立的寄存器，并允许这些寄存器在World Switch时保存恢复（又非常类似Process的上下文切换），所以虚拟机之间可以保存和恢复自己的运行环境，切换运行。

由于Guest OS与Host OS同样运行在EL1，Guest与Host的OS有同样的运行和执行权限，两个OS之间尽管在VMM管理器看来，是Host OS管理着Guest OS，但实际上却是在做着平行的切换运行。

如果Guest操作了CPU寄存器，这些寄存器会保存到自己的CPU上下文中，不同Guest之间的CPU上下文是独立运行的。

如果Guest操作了物理内存，由于EL1、EL2有两级页表，EL2的Stage-2页表会将Guest OS所访问的“物理地址”转化成最终的物理地址。Stage-2页表在EL1的KVM中调用Kernel接口申请内存和创建，并交给Hyp使用。

如果Guest操作了外设寄存器，同样由于两级页表存在，会在第二级页表触发PageFault，从而陷入到对应的虚拟化实现中，由Host OS来帮助Guest完成这次IO操作。

Guest OS的中断，由Host OS通过Hyp向Guest OS注入。ARM提供了这种虚拟中断注入机制。

对于Guest OS而言，Guest OS感知不到自己是否运行在Hyp上，与正常的OS相比，他可以自己选择什么都不改，使用全虚拟化，也可以使用virtio做半虚拟化。

Guest OS在Kernel侧，实现为一个独立线程，受正常的调度控制，当调度到这个vCPU线程运行时，Hyp会做World Switch切换到Guest OS运行，当调度中断来时，Hyp会将Guest OS切换回来，结束线程的本次时间片，切换到其他线程。当Guest OS进入Idle时，会World Switch回Host OS。

**KVM Hypervisors**

ARM平台上，VHE模式下的hyp与nVHE下的hyp是两套独立的实现，Google又提出了新的Hyp实现pkvm，所以目前KVM框架下由多个Hyp实现，启动时由arm-kvm.mode来控制使用哪个hyp。

**QEMU**

KVM并不包括Cross Platform的功能，跨平台的指令翻译，由QEMU（TCG）完成。

QEMU的基本原理可以参考：

https://lists.gnu.org/archive/html/qemu-devel/2011-04/pdfhC5rVdz7U8.pdf

## Functions

`kvm_init` `kvm_arch_init`

KVM初始化，初始化Hyp及KVM功能。

根据arm-kvm.mode、VHE是否开启、当前EL来选择使用哪个Hyp实现。

`kvm_chardev_ops`, `kvm_dev_ioctl`

- KVM_CREATE_VM

KVM节点，控制VM的创建

`kvm_vm_fops`, `kvm_vm_ioctl`

- KVM_CREATE_VCPU
- KVM_SET_USER_MEMORY_REGION
- KVM_IRQFD
- KVM_IOEVENTFD
- KVM_CREATE_DEVICE

VM节点，控制VM的初始化及运行

`kvm_device_fops`, `kvm_device_ioctl`

- KVM_SET_DEVICE_ATTR
- KVM_GET_DEVICE_ATTR
- KVM_HAS_DEVICE_ATTR
- pass through ioctl -> dev->ops->ioctl

KVM Device节点，操作KVM Device。如vgic、vfio都是独立的KVM Device。

`kvm_vcpu_fops`, `kvm_vcpu_ioctl`

- KVM_RUN
- KVM_GET_REGS
- KVM_SET_REGS

VCPU节点，控制VCPU的运行和访问操作

## Reference

ARM Virtualization

<https://developer.arm.com/documentation/102142/0100/Introduction-to-virtualization>

基于armv8的kvm实现分析（一）虚拟化介绍

<https://zhuanlan.zhihu.com/p/528599297>

基于armv8的kvm实现分析（二）armv8的虚拟化支持

<https://zhuanlan.zhihu.com/p/529084234>

基于armv8的kvm实现分析（三）kvm初始化流程

<https://zhuanlan.zhihu.com/p/530130205>

基于armv8的kvm实现分析（四）cpu虚拟化

<https://zhuanlan.zhihu.com/p/530665800>

基于armv8的kvm实现分析（五）内存虚拟化

<https://zhuanlan.zhihu.com/p/530963525>

基于armv8的kvm实现分析（六）中断虚拟化

<https://zhuanlan.zhihu.com/p/531826473>

ARM KVM Videos

<https://www.youtube.com/watch?v=oF8ydWwEuBI>

KVM Forum

<https://www.linux-kvm.org/page/KVM_Forum>

<https://www.youtube.com/@kvmforum6546>