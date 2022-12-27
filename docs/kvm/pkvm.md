# pkvm

Protected KVM（pKVM）是Google为Android实现的新的KVM nVHE Hypervisor设计。

**Background**

```
nVHE：
EL0: [    VMM  APP    ] [ APP  APP ]
EL1: [ Host OS + KVM  ] [ Guest OS ]
EL2: [           KVM Hyp           ]
```

原有的nVHE设计中，Hyp只负责简单的World Switch功能，KVM的主体功能存在于EL1的Kernel侧，所以整个Kernel，VMM（虚拟机管理器应用），以及EL2 Hyp，都在TCB（Trusted Computiong Base）中，对Guest OS有绝对的访问和控制权限。

同时，Hyp侧由于在历史上都是各家自己实现，并不在Android代码的控制范围内，导致GKI实施后，Hyp侧成了各种绕开GKI的功能“后花园”，没有人监管Hyp的代码，这里的很多代码，不像Hyp的功能，并且以更高的执行权限运行，且没有人去保证他们的安全。

实际上Andorid能控制的范围非常有限，只有Non-Secure EL0和Non-Secure EL1，而各家自己掌握的Hyp及Secure World，都不在Google的控制范围内。Android即使自身再安全，那些暴露在更高执行权限的不受控代码也很可能导致Android出现安全漏洞。

另外，Kernel很庞大，暴露的攻击面也很大，所以把Kernel、VMM都纳入到TCB里并不安全。

**Protected KVM**

```
pKVM:                isolated            NS|S
          Android       |   Protected VM   |  (Secure World)
EL0: [  VMM(crossvm)  ] | [   APP  APP   ] | [  TA   TA   TA  ]
EL1: [ Host OS + KVM  ] | [   Guest OS   ] | [     TEE OS     ]
EL2: [              pKVM Hyp             ] |
```

所以Google希望为Android提供一个新的安全模型，那就是Protected KVM。

pKVM的目的，是提供出Protected Guest VM，将VMM、Kernel从TCB中移出，只允许Protected KVM Hyp在TCB中，有绝对的管理权限，让Protected Guest VM做到独立安全。这样，Kernel与VMM将无法访问Protected Guest VM，就可以让其变成一个独立的安全沙盒。

在这个安全模型中，Host无法访问Guest的内存，当Guest申请内存时，这些内存会直接在Host端消失。

为此，Google需要扩展原有的nVHE Hyp，开发pKVM Hyp，将原有在Host OS中完成的工作移入pKVM Hyp中完成，比如Guest OS的页表创建，并让pKVM Hyp可以脱离于Host OS独立运行，做到安全独立。

最终的效果，由于pKVM包含在GKI Kernel代码中，Google可以在条件成熟时强制切换到pKVM，一统EL2世界。另外提供出的安全沙盒（Protected VM），尽管目前只包含了最基本的Minidroid小系统，但是随着更多安全APP的移入，Protected VM可以在更多功能上取代TEE，最终统一Secure World。

## Reference

Android Virtualization Framework (AVF)

https://source.android.google.cn/docs/core/virtualization?hl=zh-tw

Exposing KVM on Android, Will Deacon

https://www.youtube.com/watch?v=edqJSzsDRxk

https://mirrors.edge.kernel.org/pub/linux/kernel/people/will/slides/kvmforum-2020-edited.pdf

https://blog.csdn.net/Linux_Everything/article/details/109881719

Fuzzing Host-to Guest Attack Surface in Android Protected KVM- Eugene Rodionov & Will Deacon, Google

https://youtu.be/cJjjlSG6JEA