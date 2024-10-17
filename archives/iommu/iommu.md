# iommu

```
CPU(va)		IO Device(iova)
-----------	-------------
MMU(pa->va)	SMMU(ba->iova)
------------------------------
[	    DDR (pa)	     ]
```

iommu是iO设备与主存间的mmu，可以将io设备到内存间的ba访问转化为iova=>ba的访问。（iommu概念和原理不再赘述，可以见dma、smmu相关文章）

iommu为iommu设备抽象出了通用框架，并完成了dts配置、dma通用接口的注入，以smmu为例的iommu设备，只需要按照iommu框架实现出iommu device，剩下的dts配置通路、dma buffer的申请通路就完全ready了。

iommu设备，主要完成三件事：

1、为存在iommu的io设备，提供dts的配置方法，允许为io设备直接配置其所属的iommu设备

2、在dma-mapping通用接口中为绑定了iommu设备的io device绑定iommu-mapping流程（iova）

3、提供出通用的iommu操作接口

iommu框架包括几部分：

1、iommu device，iommu的虚基类，smmu driver是其子类

2、iova，类似vma的iova动态分配管理，包括了一个类似slab的地址空间cache

3、io-pgtable，iommu设备的页表结构，页表创建的lib库

4、of-iommu，提供出dts通用配置和解析方法

使用iommu框架，我们只需要熟悉三件事：

1、在dts中为io设备绑定iommu设备

2、dma buffer的通用申请接口（会使用所绑定的iommu设备创建mapping）

3、iommu的通用mapping接口（在驱动中手动为物理页映射iova）

一个完整的dma buffer申请流程，在为io设备绑定了iommu设备后，在使用dma_alloc_coherent等dma接口申请dma buffer时，会使用iommu的iova框架从iova空间申请出iova，然后从buddy、dma pool中申请出物理页，最后调用所绑定的iommu设备驱动所实现的map接口，为物理页创建到iova的映射，页表建立使用io-pagetable来完成。

**iommu device, iommu group, iommu domain**

iommu_device：对应一个io设备

iommu_master：对应一个smmu设备

iommu_group：共享相同IO地址空间的设备的集合（一个或多个）

iommu_domain：范围和iommu_group一样，但它定义的是group范围内对应的操作的集合

## Files

```
- /drivers/iommu/iommu.c		# iommu device & framework
- /drivers/iommu/of_iommu.c		# iommu dts properties
- /drivers/iommu/iova.c			# iommu iova management
- /drivers/iommu/io-pgtable.c		# iommu pagetable operations
```

## IOMMU Interface

iommu map、unmap interface

## IOMMU DTS Config

iommu dts config（参考：）

## Reference

IOMMU/SMMU系列文章：

<https://mp.csdn.net/mp_blog/creation/editor/122420548>