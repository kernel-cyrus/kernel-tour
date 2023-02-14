# sysctl

sysctl是内核模块开放给用户的参数配置机制，可以认为是linux下的内核控制面板，用来设置各种内核功能参数。

sysctl提供了procfs和sysctl两种操作方式：

**procfs**

sysctl机制在/proc/sys下创建出配置节点。

```
abi/		execution domains & personalities
debug/		<empty>
dev/		device specific information (eg dev/cdrom/info)
fs/		specific filesystems
		filehandle, inode, dentry and quota tuning
		binfmt_misc <Documentation/admin-guide/binfmt-misc.rst>
kernel/		global kernel info / tuning
		miscellaneous stuff
net/		networking stuff, for documentation look in:
		<Documentation/networking/>
proc/		<empty>
sunrpc/		SUN Remote Procedure Call (NFS)
vm/		memory management tuning
		buffer and cache management
user/		Per user per user namespace limits
```

可以直接通过读写节点文件进行配置，比如：

```
# echo "1" > /proc/sys/kernel/sysrq 
```

**sysctl**

一般shell环境都会提供sysctl命令，来直接操作这些配置变量:

```
# sysctl kernel.sysrq=1
```

或者直接从文件中加载配置:

```
# sysctl --system 		# will load config file from default path
# sysctl --load=filename.conf
```

## Create new sysctl node

创建新的sysctl节点，可以参考 `/lib/test_sysctl.c`

`ctl_table`

sysctl node结构体，包括name、data、proc、mode

`register_sysctl`

创建好object后，通过注册接口注册即可。

`register_sysctl_table`

这个接口可以注册一个通过.child组成的table tree。

`unregister_sysctl_table`

## Files

```
- /include/linux/sysctl.h
- /kernel/sysctl.c
- /fs/proc/proc_sysctl.c
- /fs/sysctls.c
```

## Reference

<https://wiki.archlinux.org/title/sysctl>