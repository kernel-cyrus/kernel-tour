# firmware

request_firmware用于内核中固件文件的读取。

固件文件可以是任意格式，request_firmware只用于读取固件文件的内容到fw_priv->data中。

固件文件的解析，段的加载由使用者自己完成。如果固件为通用的elf格式，则内核有通用框架可以完成elf文件的加载。

**固件存放位置**

1、编译到内核镜像中

```
CONFIG_FIRMWARE_IN_KERNEL=y
CONFIG_EXTRA_FIRMWARE_DIR="firmware"          // this means $(source_dir)/firmware
CONFIG_EXTRA_FIRMWARE="fw_sst_0f28.bin"
```

2、从文件系统加载

会从以下路径搜索文件：

```
static const char * const fw_path[] = {
	fw_path_para,
	"/lib/firmware/updates/" UTS_RELEASE,
	"/lib/firmware/updates",
	"/lib/firmware/" UTS_RELEASE,
	"/lib/firmware"
};
```

其中，fw_path_para可以通过firmware_class.path=<path>启动参数传入

**固件加载方式**

```
         request_firmware()
                 |
                 |                   Y
firmware built in kernel image? ----------->  load from kernel image	（DONE）
                 |
                 |  N (load from filesystem)
                 |
                 |                   Y                            
      load directly from fs ?   ----------->  load directly from "path"	（DONE）
                 |
                 |  N (fallback sysfs user mode)
                 |
create file node "/sys/class/firmware/xx.bin/"
                 |
                 |
                END
```

当然，除了request_firmware，还可以用request_firmware_direct直接加载并禁用fallback，也可以用request_firmware_nowait这种异步接口。

具体使用方法可以参考内核中这些接口的使用样例。

## Files

```
- /include/linux/firmware.h
- /drivers/base/firmware_loader/main.c
- /drivers/base/firmware_loader/fallback.c	# sysfs user mode helpers
```

## Reference

<https://www.kernel.org/doc/html/v4.13/driver-api/firmware/request_firmware.html>

<https://wiki.tizen.org/Usage_and_Mechanism_of_kernel_function_%22request_firmware()%22>