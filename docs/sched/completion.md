# completion

completion机制用于多核间的代码协同执行，是一种建立在调度基础上的pending-signal机制，必须在irq-on的执行上下文中使用。

## 主要接口

`init_completion`

初始化completion结构体

`wait_for_completion`

等待completion完成

`complete`

设置completion完成，并唤醒一个等待线程

`complete_all`

设置completion完成，并唤醒所有等待线程

## 典型样例

```
	CPU#1					CPU#2

	struct completion setup_done;

	init_completion(&setup_done);
	initialize_work(...,&setup_done,...);

	/* run non-dependent code */		/* do setup */

	wait_for_completion(&setup_done);	complete(setup_done);
```

## 参考资料

<https://www.kernel.org/doc/Documentation/scheduler/completion.txt>