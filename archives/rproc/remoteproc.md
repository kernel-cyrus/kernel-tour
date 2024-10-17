# remoteproc

remoteproc(Remote Processor Framework)是外核的管理框架，用于外核的上下电、固件加载等操作。

rempteproc除了定义了了用于外核管理的ops外（rproc_ops），还实现了默认的elf loader的功能，可以自动加载elf固件。

同时，remoteproc框架提供了rpmsg的功能，外核驱动只需要实现几个底层接口就可以实现rpmsg的核间通信功能。

**resource table**

remoteproc除了支持正常的elf固件的加载，也支持在固件里添加的resource table。

resource table在elf中是一个独立的.section，内部是header + list of entries的结构，entry中包括type和私有数据，可以指定需要的resource（需要让内核帮firmware做的事情）

比如：

```
static rproc_handle_resource_t rproc_loading_handlers[RSC_LAST] = {
	[RSC_CARVEOUT] = rproc_handle_carveout,		# handle phys contig memory allocation requests
	[RSC_DEVMEM] = rproc_handle_devmem,		# handle devmem resource entry
	[RSC_TRACE] = rproc_handle_trace,		# handle a shared trace buffer resource
	[RSC_VDEV] = rproc_handle_vdev,			# handle a vdev fw resource
};
```

firmware加载前，会通过`rproc_handle_resources`来完成resource table中定义的所有resource的申请。

## Structures

`struct rproc`

> represents a physical remote processor device

`struct rproc_ops`

实现一个rproc driver。

- start：用于实现power on功能
- stop：用于实现power off功能
- load：加载firmware data（如rproc_elf_load_segments）
- kick：用于实现rpmsg的vring kick功能
- coredump：用于实现子系统core dump功能

## Functions

### API for implementing rproc device

`rproc_alloc`

alloc rproc instance，并alloc出默认的elf rproc ops。

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

```
request_firmware				# load firmware file into buffer
rproc_fw_boot
	rproc_enable_iommu
		iommu_domain_alloc
		iommu_set_fault_handler
		iommu_attach_device
	rproc_prepare_device
		rproc->ops->prepare
	rproc_get_boot_addr
		rproc->ops->get_boot_addr
	rproc_parse_fw
		rproc->ops->parse_fw		# parse resource table and elf segments
	rproc_handle_resources
		loop:rproc->table_ptr
	rproc_alloc_registered_carveouts
	rproc_start
		rproc_load_segments
			rproc->ops->load	# load elf buffer to memory
                rproc_find_loaded_rsc_table	# load resource table
		rproc->ops->start
release_firmware
```

`rproc_shutdown`

下电外核

`rproc_coredump`

执行外核的coredump，会dump elf在内存中对应的各个segment，使用dev_coredump，创建一个coredump节点，供用户态访问。

`rproc_get_by_phandle`

用phandle获取rproc，refcount会+1

`rproc_put`

用完后使用，refcount-1

## Reference

<https://www.kernel.org/doc/Documentation/remoteproc.txt>