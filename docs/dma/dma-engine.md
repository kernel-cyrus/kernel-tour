## DMA Engine

DMA Engine可以表示两个概念，第一，一个DMA Engine实际就是一个硬件DMA设备的驱动（DMA Engine Device）。第二，DMA Engine在内核中是一个DMA设备驱动的通用框架（DMA Engine Framework）。

DMA Engine Framework跟大多数内核设备框架一样，DMA Engine Driver需要根据其硬件特性填充dma device结构体，并基于其硬件实现功能接口，注册到DMA Engine框架，由DMA Engine Framework向外提供统一功能接口。

通常DMA的使用者包括三类人：

1、DMA Engine Developer，开发DMA设备驱动，向系统注册DMA Engine Device，为系统提供DMA的搬运服务，比如dw-axi-dma。

2、DMA Client Developer，比如UART、USB、UFS的driver开发者，调用DMA Engine Framework提供的通用DMA操作接口，帮助其完成DMA搬运服务。

3、DMA Mapping User，利用DMA Alloc时可以建立SMMU页表的功能，创建用于与其他子系统共享的内存Buffer。

本文主要介绍前两种情况。

**DMA Engine Device**

DMA硬件是一个可以脱离CPU，独立在Memory与IO Device间做数据搬运的设备。

一个DMA硬件由多条Channel，Channel的数量由硬件决定，需要在DMA Device注册时填入到DMA Device结构体中。

DMA的搬运操作由DMA Request发起，DMA Request是一个软件虚拟概念，对应到硬件上，是一组寄存器配置操作。

DMA做搬运时，支持多种方向（direction）：MEM_TO_DEV、DEV_TO_MEM、MEM_TO_MEM，具体硬件上支持哪个方向，也需要在DMA Device初始化时配入。

搬运时，硬件支持多种搬运模式（transaction_type or cap），以dw-axi-dmac为例，该DMA Device支持DMA_MEMCPY（从连续内存搬运）、DMA_SLAVE（使用scatter-gather list搬运非连续内存）、DMA_CYCLIC（搬运ringbuffer）三种搬运模式。这些硬件支持的搬运类型，在DMA Device初始化时，使用dma_cap_set来配置。（详细见provider.rst）

DMA支持的地址，不一定是64bit，所以在device注册时，需要指定src addr、dst addr的地址位宽。

一次搬运操作支持的read或write字符数，叫做transfer width，但是一般DMA会支持将多次访问合并为一个burst（read、write operation collection），burst中支持合并多少个独立transfer操作，叫做burst的length，或者max_burst，同样在device初始化时需要定义。

DMA Engine Device除了需要填充上述硬件信息外，还需要实现DMA Device Operation：

```
device_tx_status		# poll for transaction completion
device_issue_pending		# push pending transactions to hardware (Do the transfer)
device_terminate_all		# aborts all transfers on a channel
device_pause			# pause any transfer on a channel
device_resume			# resume paused transfer on a channel
device_alloc_chan_resources
device_free_chan_resources
device_synchronize
device_config
device_prep_dma_memcpy		# get memcpy operation desc
device_prep_slave_sg		# get slave operation desc
device_prep_dma_cyclic		# get cyclic operation desc
```

最后通过 `dmaenginem_async_device_register` 注册到DMA Engine Framework中。

Reference: <https://www.kernel.org/doc/html/latest/driver-api/dmaengine/provider.html>

**DMA Engine Client**



Reference: <https://www.kernel.org/doc/html/latest/driver-api/dmaengine/client.html>

## Files

```
- /drivers/dma/dmaengine.c	# dma engine framework implementation
- /include/linux/dmaengine.h	# dma engine framework implementation
				# also interfaces for both engine and client driver
- /drivers/tty/serial/8250/8250_dma.c			# dma client device driver
- /drivers/dma/dw-axi-dmac/dw-axi-dmac-platform.c	# dma engine device driver
```

## Structure

```
struct dma_device
```

## Variable

`dma_device_list`

dma engine device全局链表

## Functions

`dma_bus_init`

初始化unmap pool，debugfs节点，sysfs节点

## Interface（for DMA Engine）

\* Reference Driver: dw-axi-dma

## Interface（for DMA Client）

\* Reference Driver: 8250_dma

**Request a channel**

`dma_request_slave_channel_compat`

`dma_release_channel`

**Get channel's device caps**

`dma_get_slave_caps`

**Config the channel**

`dmaengine_slave_config`

**Pause / Resume DMA transfer**

`dmaengine_pause`

`dmaengine_resume`

**Prepare dma transfer descripter**

`dmaengine_prep_dma_memcpy`

`dmaengine_prep_slave_single`

`dmaengine_prep_dma_cyclic`

**Do the transfer**

`dmaengine_submit`

`dma_async_issue_pending`

**Terminate all transfer**

`dmaengine_terminate_async`

`dmaengine_terminate_sync`

## Debugfs

`dmaengine/summary`

打印出DMA设备列表，每个DMA设备的channel数，是否正在使用。

`dmaengine/<dma_device>/`

调用每个DMA设备自己的dump功能打印

## Sysfs

`/sys/devices/system/dma/<dma_device>`

- memcpy_count
- bytes_transferred
- in_use：chanels in use

## Reference

How to implement a dma engine device driver:

<https://docs.kernel.org/driver-api/dmaengine/provider.html>

How to use dma in dma client device driver:

<https://docs.kernel.org/driver-api/dmaengine/client.html>

How to test dma client device with dmatest:

https://docs.kernel.org/driver-api/dmaengine/dmatest.html<>