# debugobject

debug object是内核为各类机制实现提供的内部object生命周期监控机制。

他可以对object的init、alloc、free的过程进行监控，避免发生double free、init after free等问题。

这个机制，非常适合于对外提供object申请的各类机制，由于object的使用是在外部，可以为外部使用的合理性进行监控。

当发生问题时，会触发 `debug_print_object` 报出问题。

## Files

```
- /lib/debugobjects.c				# Implementation
- /include/linux/debugobjects.h			# Interface
- /Documentation/core-api/debug-objects.rst	# Document
```

## Reference

<https://www.kernel.org/doc/html/latest/core-api/debug-objects.html>