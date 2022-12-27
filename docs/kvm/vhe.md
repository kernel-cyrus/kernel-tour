# VHE

### X86 vs ARM

x86对VM的实现与ARM非常不一样

x86将CPU世界分为Root(for Hyp) / Non-Root(for VM)，通过VM entry/exit来切换CPU世界

ARM将CPU世界分为EL1（for Kernel and VM）、EL2（for Hyp），通过SVC Call来切换EL

EL2是专门为Hyp增加的运行模式，not designed for full os，主要体现在virtual memory的支持和el0交互功能要比EL1少很多。

### Type-1 vs Type-2 Hypervisor

Type-1 hyp，hyp系统直接运行在硬件上，hyp上面运行独立的guest os，不需要host os，如：XEN，VMware ESXi，Microsoft Hyper-V

Type-2 hyp，hyp运行在host os里，guest os使用hyp，运行在host os上，如：KVM，Virtual Box，VMware Workstation

不论在x86或ARM上，这些hyp都有对应硬件平台的实现。

Type-1模式下(XEN)：

```
EL0: [ APP  APP ] [ APP  APP ]
EL1: [ Guest OS ] [ Guest OS ]
EL2: [        XEN Hyp        ]
```

Type-2模式下(KVM)：

```
EL0: [    APP  APP    ] [ APP  APP ]
EL1: [ Host OS + KVM  ] [ Guest OS ]
EL2: [           KVM Hyp           ]
```

在KVM中(type1 hyp)，一个VM访问，是从VM <-> Hyp <-> Kernel（hyp对vm提供服务，最终找os来完成操作） 

在ARM上，kernel是运行在EL1的，kvm的部分功能，必须使用EL2来实现（switch the world between host and guest），所以这就导致一次VM访问，需要通过VM(EL1) <-> Hyp(EL2) <-> Kernel(EL1)在Host、Guest、Hyp间经过几次EL和Switch切换。

### VHE（Virtual Host Extention）

```
EL0: [    APP  APP    ] [ APP  APP ]
EL1: [                ] [ Guest OS ]
EL2: [    Host OS + KVM + Hyp      ]  (VHE Mode)
```

如果可以让Kernel和KVM Hyp同时运行在EL2，就可以避免重复的EL切换和Switch的操作。

由于Kernel is designed to run EL1，并且EL2 is not designed for running full os，简单将Kernel放到EL2运行是不可行的（需要一套独立且复杂的实现）。

ARM为这个方案提供了新的硬件特性——VHE。VHE允许Kernel在EL2运行，同时，对Kernel和APP来说，又感知不到其运行在EL2，无需修改任何代码。

具体ARM VHE特性的实现，主要来讲，在配置开启VHE特性后，EL2增加了原来相比EL1没有的功能，满足了OS的运行条件，如EL1的Virtual Memory功能，EL1的Timer，同时兼容了原来在EL1运行的代码，如原来EL1寄存器、中断到EL2的路由，所以可以做到Kernel在VHE模式下，所有代码都不需要做任何修改。

可以说（从名字上也可以看出），VHE是专门用于优化这种Type2类型的虚拟化方案而诞生的。

切换VHE模式后，EL1相当于与EL2合并了，EL1不再运行任何代码，EL2进入了VHE模式。

这里插播个小故事，ARM在实现VHE硬件特性前，Linaro为了验证这个想法的可行性，修改了一个可以在EL2运行的Linux，验证了这个想法的性能（佩服啊）。

### VHE vs nVHE

VHE和nVHE在不同的地方可能有不同的含义。

开启了VHE特性，并将Kernel和KVM放入EL2，KVM使用VHE的实现，叫做VHE模式。

不开启VHE特性，或者在不支持VHE特性的ARM平台上，以传统Split的方式运行Kernel和KVM，KVM使用独立的Hyp实现并与Kernel通过EL切换的方式进行通信，叫做nVHE模式，或者Hyp模式。

在ARM平台的KVM的实现中，vhe和nvhe的是两个独立的hyp实现。

所以，vhe或者nvhe这两个词，可以用于描述一个硬件平台是否支持vhe特性，也可以用来说明kernel与kvm是以那种方式运行，还可以用来说明kvm使用的是哪种hyp实现。

### Enter VHE Mode

硬件上配置VHE是否启用VHE，是通过配置HCR_EL2寄存器来完成的。

5.10 Kernel，需要开启CONFIG_ARM64_VHE=y来让Kernel进入VHE模式

5.15 Kernel，不要传入kvm-arm.mode参数，并在EL2启动Kernel（待验证）

kvm-arm.mode=nvhe，会通过bootargs传入id_aa64mmfr1.vh=0，禁用掉CPU的VHE Feature，从而禁止CPU初始化进入VHE，kvm_mode会设置为KVM_MODE_DEFAULT，进入nvhe模式，或叫hyp模式

kvm-arm.mode=protected，会通过bootargs传入id_aa64mmfr1.vh=0，禁用掉CPU的VHE Feature，从而禁止CPU初始化进入VHE，kvm_mode会设置为KVM_MODE_PROTECTED，进入pkvm（protected kvm）模式，或者叫protected nvhe模式。

kvm-arm.mode=none，则kvm_mode会设置为KVM_MODE_NONE，动态禁用掉KVM功能。

kvm-arm.mode不设置，则kvm_mode为默认的KVM_MODE_DEFAULT，从EL2进入会进入VHE模式。

### Reference

ARM Virtual Host Extensions

https://developer.arm.com/documentation/102142/0100/Virtualization-Host-Extensions

To EL2 and beyond, Christoffer Dall from Linaro

https://www.youtube.com/watch?v=j0bp4fnO98w

http://events17.linuxfoundation.org/sites/events/files/slides/To%20EL2%20and%20Beyond_0.pdf