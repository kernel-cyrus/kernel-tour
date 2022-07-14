# params

## Add bootargs

`early_param`

builtin的内核代码，可以在内核代码任何位置使用 `early_param` 来注册新的bootargs。

每个bootargs，对应一个handler，来解析这个bootargs的value。

如果cmdline传入了注册的bootargs，内核启动阶段 `parse_early_param` 时，会逐一执行各个bootargs对应的handler，完成bootargs的初始化。

## Add module param

`module_param`, `module_param_array`

module的启动参数，可以使用 `module_param` 来定义，每个参数被定义为一个变量，module初始化时，变量会被赋值。

如果module编译为ko，这些参数可以在insmod时传入。

如果module是builtin的，这些参数可以通过cmdline，以"[module].[param]=xxx"的形式传入

## Test

[test-params](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-params)