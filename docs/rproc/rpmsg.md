# rpmsg

rpmsg是基于virtio实现的核间通信机制。

可以认为rpmsg是在virtio queue通信链路基础上，实现了一个薄薄的协议层，并在内核中提供了一个rpmsg的消息通信框架

从文件实现上，可以清晰划分出rpmsg的分层实现

**rpmsg_core.c**

创建了rpmsg bus，用于注册rpmsg device、rpmsg driver。bus提供了driver/device match和probe机制，在match后，通过probe实例化出device。这里并未创建任何rpmsg device，也未实现任何rpmsg driver。

定义了rpmsg device，一个rpmsg device就是一个核间通信的通路（channel）

一个device有一个默认rpmsg endpoint（逻辑对端），用于执行实际的send操作（rpmsg_endpoint_ops），也可以创建额外的endpoint。每个endpoint包含一个地址及callback，virtio rpmsg bus会将消息路由给对应的endpoint，最终调用callback来处理消息。

定义了rpmsg driver结构，client rpmsg driver可以在probe中创建出出自己rpmsg device（比如sme rpmsg driver，创建出sme rpmsg device），这里主要的工作是实现rpmsg device中定义和使用的ops。


最后，在内核态，提供了rpmsg的注册、发送接口

在用户态，rpmsg bus对probe的devie在sys/bus/rpmsg/下提供了操作节点

**rpmsg_char.c**

rpmsg char device driver，可以把注册到rpmsg bus上的rpmsg device，probe为一个char device，在用户空间提供出file操作节点，用于消息发送。

rpmsg device在注册到rpmsg总线前已经被初始化（相关ops）。

**rpmsg_ctrl.c**

rpmsg ctrl device driver，可以把注册到rpmsg bus上的rpmsg device，probe为一个char device，在用户空间提供出ioctrl操作节点，用于消息发送。

rpmsg device在注册到rpmsg总线前已经被初始化（相关ops）。

**virtio_rpmsg_bus.c**

基于virtio的传输框架。

实现了ipc(rpmsg) virtio driver，为注册到virtio bus上的rpmsg virtio device，probe出rpmsg device，包括初始化出rpmsg device的rpmsg_device_ops（virtio_rpmsg_ops）和rpmsg_endpoint_ops（virtio_endpoint_ops）。

## Files

```
- /drivers/rpmsg/rpmsg_core.c
- /drivers/rpmsg/rpmsg_char.c
- /drivers/rpmsg/rpmsg_ctrl.c
- /drivers/rpmsg/virtio_rpmsg_bus.c
```

## Reference

<https://wiki.st.com/stm32mpu/wiki/Category:Coprocessor_management_Linux>