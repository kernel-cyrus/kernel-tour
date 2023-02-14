# bootconfig

Kernel Cmdline因为受到长度和格式比较受限，不能处理更复杂的参数格式。

bootconfig就是解决这个问题而诞生的。

bootconfig是一个append在initramfs后面的纯文本文件，加载后的layout如下：

```
[      initrd      ]
[    bootconfig    ]
[     padding      ]
[    size(le32)    ]
[  checksum(le32)  ]
[   #BOOTCONFIG\n  ]
```

可以使用如下命令将写好的bootconfig文件append到initrd中

```
# tools/bootconfig/bootconfig -a your-config /boot/initrd.img-X.Y.Z
```

启动时，需要在cmdline中添加"bootconfig"来告诉kernel使能bootconfig

bootconfig在start_kernel阶段，由 `setup_boot_config` 来加载，加载成功后，会打印如下Log：

```
Load bootconfig: xxx bytes xxx nodes
```

启动后，在 `/proc/bootconfig` 中，也可以查看加载的bootconfig

Debug时，可以通过`saved_boot_config`变量来获取加载的bootconfig。

## Files

```
- /fs/proc/bootconfig.c
- /include/linux/bootconfig.h
```

## Reference

bootconfig

<https://docs.kernel.org/admin-guide/bootconfig.html>

android bootconfig

<https://source.android.com/docs/core/architecture/bootloader/implementing-bootconfig>

using boot time ftrace with bootconfig

<https://lwn.net/Articles/806002/>