# smmu

smmu是arm平台的iommu，kernel中的smmu是arm smmu的driver。

smmu的基本概念和原理可以参考后续整理的ARM相关笔记，或者下文：

<https://zhuanlan.zhihu.com/p/534550409>

smmu driver实现了几部分功能：

1、实现了iommu device

2、实现了smmu in-memory数据结构的创建接口

3、实现了smmu cmd、event queue操作接口

4、实现了smmu irq、event、error的handling

smmu最终创建页表，使用的是io-pgtable提供的通用接口。iommu将页表维护的这部分功能也通用化了。

## Files

```
- /drivers/iommu/arm/arm-smmu-v3/arm-smmu-v3.c
```

## Structures

`struct arm_smmu_device`

## Variables

`struct iommu_ops arm_smmu_ops`

iommu接口的smmu实现（iommu继承类）

主要回调包括：domain_alloc、probe_device、device_group、map_pages

具体接口定义见iommu。

`struct iommu_flush_ops arm_smmu_flush_ops`

iommu tlb flush接口的smmu实现

## Functions

`arm_smmu_device_probe`

smmu的probe函数功能非常清晰，匹配smmu compatible后，先获取并初始化smmu硬件寄存器，注册中断（处理smmu异常和event），初始化smmu使用的in memory data structure，主要是cmd queue和event queue，然后创建iommu设备，注册到iommu框架。