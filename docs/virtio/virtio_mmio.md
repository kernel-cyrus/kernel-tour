# virtio mmio

userspace -> operate xx device node

xx device framework -> xx device node

virtio driver (bt, gpu, input...) -> match driver with device, probe the particular driver device (use virtqueue to implement xx device driver and register to the xx device framework)
-> register (virtio driver)

virtio device -> register the device instanse to the virtio bus

virtio mmio (from standard virtio device regs layout -> config ops, the device operations)
virtio_remoteproc
-> register (virtio device)

the xx virtio device driver will do the fdt walk and create device for each node

