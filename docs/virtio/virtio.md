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

**Virtio device & driver**

Virtio driver，在Guest OS中实现，也叫做front-driver。Kernel的很多driver框架，都有virtio device的driver实现。

Virtio device，在QEMU中实现，也叫做backend-driver，用于相应driver的请求，在Host OS中完成功能，并回传结果给front-driver。

Virtio driver与Virtio device之间，通过virtio queue、virtio ring完成通信。

## Files

```
/include/linux/virtio.h
/drivers/virtio/virtio.c
```

## Structures

`virtio_device`

`virtio_driver`

`virtqueue`

## Functions

`virtio_init`

`register_virtio_device`

`register_virtio_driver`

`virtio_device_of_init`

`virtio_dev_match`

`virtio_dev_probe`

## File Nodes

`/sys/bus/virtio/devices/<device>`

- device
- vendor
- status
- modalias
- features

## Reference

Virtio: An I/O virtualization framework for Linux

https://developer.ibm.com/articles/l-virtio/

Introduction to VirtIO (with full example)

https://blogs.oracle.com/linux/post/introduction-to-virtio