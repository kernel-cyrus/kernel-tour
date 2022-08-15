# kobject

内核的很多模块都使用了面向对象的思想。kobject就是一个典型的例子。

kobject是kernel object，是所有设备类的基类，类似于各类framework框架中的base object基类的概念。

C语言中通过在自己的结构体中嵌入kobject，来实现对kobject的继承。

每个kobject，都包含了自己的ktype，表示kobject的type，或者说子类的类型。

ktype定义了kobject基类的虚函数接口，这些虚函数由子类来实现，这里定义了类似构造及虚构函数的接口, sysfs的ops。

kset是一个kobject的集合，kset可以将kobject平行组织，kset自身也是一个kobject（通过内嵌kobject实现的子类），kobject可以通过parent指针来纵向组织，这样kobject及kset实现了一个树状结构。

sysfs通过文件系统，将kset（实际也是一种kobject）、kobject组成的拓扑结构展示了出来，并提供了对各类kobject进行操作的接口。需要注意的是，kobject并不会自动创建出对应的sysfs节点，kobject在sysfs中的节点，是调用sysfs接口为其创建的，这不过这类创建过程被类似device框架封装了。

kobject基类的好处，就是可以将系统中各种各样类别的设备组织在一起，并将所有object公共的功能，分层实现在继承链路中各级基类中，比如我们看到的sysfs、ref count、uevent等功能。

## Files

```
- /include/linux/kobject.h	// kobject base class interface
- /lib/kobject.c		// kobject base class implementation
```

## Functions

`kobject_name` get name

`kobject_set_name` set name

`kobject_init` init kobject

`kobject_add` add kobject to parent

`kobject_del` remove kobject from parent

`kobject_move` move kobject to another parent

`kobject_get` ref count +1

`kobject_put` ref count -1

`kobject_get_ownership`

`kobject_get_path` get path from kobject chain

`kobject_namespace`

`kset_get`

`kset_put`

`kset_find_obj`

`kobject_uevent`

## Module Test

[test-kobject](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-kobject)

## Reference

<https://www.bilibili.com/read/mobile?id=16494023&share_from=article&share_medium=android&share_plat=android&share_session_id=3353d765-b262-4eb3-89a4-9b139345ed81&share_source=COPY&share_tag=s_i&timestamp=1660491094&unique_k=SOYhDYu>