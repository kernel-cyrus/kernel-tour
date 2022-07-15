# params

cmdline启动参数及module参数的定义和使用。

定义的参数，可以通过cmdline传入，由对应handler解析，也可以在insmod时传入，并在sysfs下提供出读写接口。

## Files

```
- include/linux/init.h		# early_param
- include/linux/moduleparam.h	# module_param, core_param
```

## Interfaces

### early_param

可以在内核代码任何位置使用 `early_param` 来注册新的bootargs。

每个bootargs，对应一个handler parser，来解析这个bootargs的value。

内核启动阶段在 `parse_early_param` 时，会逐一解析cmdline中的bootargs，调用handler完成bootargs的初始化。

### module_param

module的启动参数，可以使用 `module_param` 来定义，每个参数被定义为一个变量，module初始化时，变量会被赋值。

如果module编译为ko，这些参数可以在insmod时通过"\<param\>=xxx"传入。

如果module是builtin的，这些参数可以通过cmdline，以"\<module\>.\<param\>=xxx"的形式传入

`module_param` 同时会在 `/sys/module/<module>/parameters` 下创建文件节点供用户查看和修改。

文件节点的权限由 `module_param` 最后一个perm参数传入，见 `/include/linux/stat.h`

### core_param

`core_param` 用于在module中定义类似 `early_param` 这类内核参数。

如果通过cmdline传参，格式与 `early_param` 一样，直接使用"\<params\>=xxx"即可。

`core_param` 与 `module_param` 类似，会在 `/sys/module/kernel/parameters/` 下创建文件节点。

## Test

[test-params](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-params)