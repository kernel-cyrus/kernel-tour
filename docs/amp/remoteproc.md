# remoteproc

remoteproc(Remote Processor Framework)是外核的管理框架，用于外核的上下电、固件加载等操作。

remoteproc框架同时提供了rpmsg的功能，外核驱动只需要实现几个底层接口就可以实现rpmsg的核间通信功能。

## Structures

`struct rproc`

> represents a physical remote processor device

`struct rproc_ops`

- start：用于实现load firmware、power on功能
- stop：用于实现power off功能
- kick：用于实现virtio queue kick回调，用于通知外核

实现一个rproc driver。

## Functions

### API for implementing rproc device

`rproc_alloc`

alloc rproc instance

`rproc_free`

free rproc instance

`rproc_add`

add rproc to framework

`rproc_del`

remove from rproc framework

`rproc_report_crash`

report a crash

### API for using rproc device

`rproc_boot`

启动一个核（加载固件，上电运行）

`rproc_shutdown`

下电外核

`rproc_get_by_phandle`

用phandle获取rproc，refcount会+1

`rproc_put`

用完后使用，refcount-1