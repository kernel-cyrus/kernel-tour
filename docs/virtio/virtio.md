# virtio

Virtio是virtio设备的驱动框架，用来实现和管理virtio device driver。

virtio不仅是kernel中的一个虚拟化设备驱动的框架，同时也成为了一套规范。使用这套规范，不同的Hyp可以实现出相同的Virtio Device，不同的Guest OS，也可以实现出对应Virtio Device的Driver，这样Guest OS就可以搭配不同的Hypervisor运行了。

这里的Hypervisor指QEMU，并非ARM Trustzone中的Hypervisor。

**Full-virtualization vs Para-virtualization**

virtio是一套半虚拟化框架实现。

```
Full-virt:				Para-virt:
-----------------------------		---------------------------------
[         Guest OS          ]		[           Guest OS            ]
[ real driver | real driver ]		[ virtio driver | virtio driver ]
-----------------------------		---------------------------------
[  emu device |  emu device ]		[ virtio device | virtio device ]
[      Hypervisor(QEMU)     ]		[        Hypervisor(QEMU)       ]
-----------------------------		---------------------------------
[          Host OS          ]		[            Host OS            ]
[ host driver | host driver ]		[   host driver | host driver   ]
-----------------------------		---------------------------------
[       Host Hardware       ]		[         Host Hardware         ]
-----------------------------		---------------------------------
```

全虚拟化：也叫emulation，guest vm使用真实硬件驱动，hypervisor直接模拟出真实硬件的interface完成仿真。

半虚拟化：也叫virtualization，通过virtio规范，hypervisor提供出各类virtio device，guest os使用virtio device driver来驱动。这种方式效率更高。

Virtio driver，在Guest OS中实现，也叫做front-driver。Kernel的很多driver框架，都有virtio device的driver实现。

Virtio device，在QEMU中实现，也叫做backend-driver，用于相应driver的请求，在Host OS中完成功能，并回传结果给front-driver。

Virtio driver与Virtio device之间，通过virtio queue完成通信。

Device的信息、配置、操作接口，甚至vq，都通过virtio config(mmio)传递提供给driver。

**Virtio device driver**

virtio device driver框架非常类似于platform device driver框架，供了virtio bus，定义了virtio device和virtio driver，允许device和driver注册到总线上，并通过match来匹配。device可以通过dts定义，各类virtio设备可以实现对应的virtio driver（比如mmio、keyboard、block等virtio device driver）。virtio device在dts解析时被创建并注册到virtio bus上，各类virtio driver在自己driver init时注册到virtio bus上，当virtio_device_id match时，用driver probe device，创建出virtio所实现的对应设备类型的设备。

以一个典型的virtio device driver为例：

首先，backend virtio device有哪些，硬件环境是固定的（虚拟机的硬件配置）。

DTS中要定义这些Device的节点，来描述硬件环境。

通常情况下，virtio device是基于mmio的，可以用standard mmio reg layout来访问和操作device。

virtio mmio driver本身是个platform device driver，这让他具有了dts compatible匹配及probe的功能。这样，我们只需要在compatible上注明"virtio,mmio"。virtio mmio driver就会通过通过标准化的mmio reg interface，帮我们读取backend device信息，然后创建出virtio device，同时，为virtio device赋予config的能力（通过mmio访问和操作backend device），最后注册到virtio bus上。

virtio bus是用来匹配virtio device和virtio driver的bus，通过virtio_device_id来match，这个id由mmio在向virtio bus上创建注册virtio device前获得。

一个virtio device driver，一定是某个类型device的driver，比如具体是一个input、blk、net、console等。

一个具体device类型的driver，kernel一般都有这个类型driver的通用框架，并抽象了这个类型的device结构体、及driver操作。某一具体device类型的virtio device driver需要调用virtio queue、virtio config，实现出该类型的device的ops操作，并创建出该类型的device的实例，注册到该类型的device框架中，从而为上层提供出该类型标准的操作节点及接口。

virtio queue、virtio config分别见virtio_ring、virtio_mmio

## Files

```
/include/linux/virtio.h
/drivers/virtio/virtio.c
```

## Variables

`virtio_bus`

virtio设备总线，允许device、driver注册到总线上，并提供出/sys/bus/virtio总线节点，用于生成注册上来的device和driver节点。通过提供match接口，在device和drvier match后，调用probe用匹配driver初始化device。

## Structures

`virtio_device`

virtio设备

其中，config_change, virtio_device_id, virtio_config_ops都是由backend driver提供？front-driver可以直接使用这些接口读取host device的配置。比如获取支持的feature，或者利用config来读写操作decice配置。（不确定）

`virtio_driver`

virtio设备驱动，各类virtio device可以实现自己的virtio driver

driver主要提供probe功能，以及设备端config changed回调。

`virtqueue`

virtio底层消息队列

## Functions

`virtio_init`

初始化virtio bus。

`register_virtio_device`

简单的初始化virtio device，把创建的device挂到virtio bus下（在/sys/bus/virtio/创建device节点）

如果dts中有这个virtio device的节点，则把np绑定到device上。（``virtio_device_of_init``）

`register_virtio_driver`

把virtio driver挂到virtio bus上。

`virtio_dev_match`

device、driver都会注册到virtio_bus上，在device和driver match后，调用probe用driver初始化device。

`virtio_dev_probe`

调用virtio driver来初始化virtio device

`cread_xxx`、`cwrite_xxx`

使用backend device提供的config ops，来获取或写入device配置。

## File Nodes

`/sys/bus/virtio/devices/<device>`

- device
- vendor
- status
- modalias
- features

## Reference

Virtio: An I/O virtualization framework for Linux

<https://developer.ibm.com/articles/l-virtio/>

Introduction to VirtIO (with full example)

<https://blogs.oracle.com/linux/post/introduction-to-virtio>

virtio 虚拟化系列之一：从 virtio 论文开始

<https://zhuanlan.zhihu.com/p/68154666>