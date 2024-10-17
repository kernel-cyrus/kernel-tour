# virtio ring

virtio ring是virtio driver(front-driver, guest driver)和device(backend-driver, qemu device)间的通信队列。

一般virtqueue的通用操作为：

1、申请一个virtqueue，绑定virtqueue的notify，callback分别用于kick和接收respnse

2、向virtqueue add buffer

3、执行virtqueue的kick，通知对端

4、polling used buffer，处理释放

5、如果有中断号，可以注册中断回调，在回调中通过vring_interrupt调用callback

可以参考virtio_bt.c，virtio_mmio.c的实现。

**split vs packed ring**

（待完成）

## Files

```
/drivers/virtio/virtio_ring.c
```

## Structures

`struct virtqueue`

- list：all virtqueues for the device
- callback：called when buffer consumed
- name: name
- vdev：virtio device belongs to
- index：?
- num_free：
- priv：private data

## Interfaces

`vring_create_virtqueue`

创建virtqueue，绑定virtqueue所属virtio device，绑定notify(kick)及callback

根据virtio device feature是否具有VIRTIO_F_RING_PACKED，选择创建packed或split vq。

创建时，会调用 `vring_alloc_queue` 根据device feature来决定queue所使用的内存是dma buffer还是普通内存。

`vring_new_virtqueue`

使用驱动自己的内存创建vq。

`virtqueue_add_outbuf`, `virtqueue_add_inbuf`

向queue中添加buffer。

`virtqueue_get_buf`, `virtqueue_get_buf_ctx`

get unused buffer

`virtqueue_kick`, `virtqueue_notify`

调用vq绑定的kick回调，通知远端消费buffer。

`virtqueue_detach_unused_buf`

## Reference

<https://www.redhat.com/en/blog/virtio-devices-and-drivers-overview-headjack-and-phone>

<https://www.redhat.com/en/blog/virtqueues-and-virtio-ring-how-data-travels>
