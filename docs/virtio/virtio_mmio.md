# virtio mmio

mmio，就是memory mapped io，也就是通过映射的物理地址访问设备寄存器。我们正常看到的大多数device，都是mmio方式访问的。与mmio对一个的是pmio（port mapped io）。

virito mmio，是将backend device的操作，映射到了guest的pa上，与普通设备直接映射寄存器不同，这里对各种virtio backend device定义了统一的mmio访问layout，然后将这个统一的操作layout映射到了guest os。这样，操作任何一种backend virtio device，都可以用同一个"reg" layout来操作，这个layout提供出来的所有操作，被封装成了virtio_mmio_config_ops。

这样，virtio mmio定义出了guest os与backend device间操作的标准化操作规范，所有virtio device，都需要按这个协议来对guest os提供接口。

virtio mmio driver是一个platform driver，其作用是通过compatble（"virtio,mmio"）来匹配找到基于mmio实现的backend device，获取device信息，创建出对应的virtio device实例，并注册到virtio bus上。

创建出的virtio device，会包括mmio的config操作，用来访问配置backend device，同时会获得virtio device id，用来标识这个backend device是一个什么物理设备，virtio bus会用这个id为其匹配driver。

## Files

```
- /drivers/virtio/virtio_mmio.c			# mmio interface
- /include/uapi/linux/virtio_mmio.h		# standard mmio reg layout
```