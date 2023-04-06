https://www.cnblogs.com/arnoldlu/p/8051674.html
https://blog.csdn.net/weixin_45636061/category_12041410.html


earlymem：fixmap、memblock
页表管理：mmu、memory(pagetable)、pagewalk、ptdump、pgtable-generic
物理内存：page_alloc(buddy)、slab/slub/slob
虚拟内存：vmalloc、mmap、vmacache
debug：kfence、kasan
swap: swap...
other：per_cpu、mempool、usercopy
dma：cma、dmapool
control: memcg ()
oom、oom_kill
early_ioremap、ioremap
lru、multigen-lru
ksm
page_poison
kswap
zswap = zram + swap
cgroup: 

文件页：file backend page（因为文件中有备份，内存中的page可以随时被释放）
脏页：被修改但是又没有刷到文件里的文件页（要刷完才能释放）
匿名页：https://www.bbsmax.com/A/kjdwLWj2zN/

内存紧张时，从内存到file，叫内存回收（page reclaim），内存回收基于LRU
访问回收的page时，发生缺页中断，触发页面换回（page_swap）

申请内存出现不足，首先进行直接内存回收
除此之外，内核还有线程专门在后台监控内存使用水线，可用内存过低时会主动触发内存回收（kswapd）
如果reclaim还不能获得足够内存，则会触发内存规整：pages_compact（内存碎片整理）
如果还不行，则进行OOM，杀进程

[changed?]   [memory]  [disk]

页迁移（migrate）用于memory hotplug，将物理页从一个node迁移到另一个node上（copy内容，更新映射）



------------------------------
-rw-rw-r-- 1 cyrus cyrus  462488 6月  28  2022 ./backing-dev.o
-rw-rw-r-- 1 cyrus cyrus  191272 6月  28  2022 ./balloon_compaction.o
-rw-rw-r-- 1 cyrus cyrus  162904 6月  28  2022 ./cleancache.o
-rw-rw-r-- 1 cyrus cyrus  134920 6月  28  2022 ./cma_debug.o
-rw-rw-r-- 1 cyrus cyrus  351560 6月  28  2022 ./cma.o
-rw-rw-r-- 1 cyrus cyrus  819864 6月  28  2022 ./compaction.o
-rw-rw-r-- 1 cyrus cyrus  313648 6月  28  2022 ./damon/core.o
-rw-rw-r-- 1 cyrus cyrus  283456 6月  28  2022 ./damon/paddr.o
-rw-rw-r-- 1 cyrus cyrus  254288 6月  28  2022 ./damon/prmtv-common.o
-rw-rw-r-- 1 cyrus cyrus  162960 6月  28  2022 ./damon/reclaim.o
-rw-rw-r-- 1 cyrus cyrus  265992 6月  28  2022 ./debug.o
-rw-rw-r-- 1 cyrus cyrus  171576 6月  28  2022 ./dmapool.o
-rw-rw-r-- 1 cyrus cyrus   28488 6月  28  2022 ./early_ioremap.o
-rw-rw-r-- 1 cyrus cyrus  257080 6月  28  2022 ./fadvise.o
-rw-rw-r-- 1 cyrus cyrus 1205632 6月  28  2022 ./filemap.o
-rw-rw-r-- 1 cyrus cyrus  554680 6月  28  2022 ./gup.o
-rw-rw-r-- 1 cyrus cyrus     968 6月  28  2022 ./highmem.o
-rw-rw-r-- 1 cyrus cyrus 1090304 6月  28  2022 ./huge_memory.o
-rw-rw-r-- 1 cyrus cyrus  111808 6月  28  2022 ./init-mm.o
-rw-rw-r-- 1 cyrus cyrus  254880 6月  28  2022 ./interval_tree.o
-rw-rw-r-- 1 cyrus cyrus  306528 6月  28  2022 ./kasan/common.o
-rw-rw-r-- 1 cyrus cyrus  254848 6月  28  2022 ./kasan/hw_tags.o
-rw-rw-r-- 1 cyrus cyrus    9360 6月  28  2022 ./kasan/report_hw_tags.o
-rw-rw-r-- 1 cyrus cyrus  300632 6月  28  2022 ./kasan/report.o
-rw-rw-r-- 1 cyrus cyrus    4312 6月  28  2022 ./kasan/report_tags.o
-rw-rw-r-- 1 cyrus cyrus  215544 6月  28  2022 ./kasan/tags.o
-rw-rw-r-- 1 cyrus cyrus  363104 6月  28  2022 ./kfence/core.o
-rw-rw-r-- 1 cyrus cyrus  245760 6月  28  2022 ./kfence/report.o
-rw-rw-r-- 1 cyrus cyrus  818352 6月  28  2022 ./khugepaged.o
-rw-rw-r-- 1 cyrus cyrus  274912 6月  28  2022 ./list_lru.o
-rw-rw-r-- 1 cyrus cyrus  163528 6月  28  2022 ./maccess.o
-rw-rw-r-- 1 cyrus cyrus  567432 6月  28  2022 ./madvise.o
-rw-rw-r-- 1 cyrus cyrus  308544 6月  28  2022 ./memblock.o
-rw-rw-r-- 1 cyrus cyrus 1619928 6月  28  2022 ./memcontrol.o
-rw-rw-r-- 1 cyrus cyrus  293672 6月  28  2022 ./memfd.o
-rw-rw-r-- 1 cyrus cyrus  446448 6月  28  2022 ./memory_hotplug.o
-rw-rw-r-- 1 cyrus cyrus 1197008 6月  28  2022 ./memory.o
-rw-rw-r-- 1 cyrus cyrus  271368 6月  28  2022 ./mempool.o
-rw-rw-r-- 1 cyrus cyrus  725752 6月  28  2022 ./migrate.o
-rw-rw-r-- 1 cyrus cyrus  296408 6月  28  2022 ./mincore.o
-rw-rw-r-- 1 cyrus cyrus  396272 6月  28  2022 ./mlock.o
-rw-rw-r-- 1 cyrus cyrus  354688 6月  28  2022 ./mmap_lock.o
-rw-rw-r-- 1 cyrus cyrus  713784 6月  28  2022 ./mmap.o
-rw-rw-r-- 1 cyrus cyrus  160272 6月  28  2022 ./mm_init.o
-rw-rw-r-- 1 cyrus cyrus  260984 6月  28  2022 ./mmu_gather.o
-rw-rw-r-- 1 cyrus cyrus  209376 6月  28  2022 ./mmu_notifier.o
-rw-rw-r-- 1 cyrus cyrus  220344 6月  28  2022 ./mmzone.o
-rw-rw-r-- 1 cyrus cyrus  324624 6月  28  2022 ./mprotect.o
-rw-rw-r-- 1 cyrus cyrus  328248 6月  28  2022 ./mremap.o
-rw-rw-r-- 1 cyrus cyrus  204640 6月  28  2022 ./msync.o
-rw-rw-r-- 1 cyrus cyrus  682504 6月  28  2022 ./oom_kill.o
-rw-rw-r-- 1 cyrus cyrus 1249128 6月  28  2022 ./page_alloc.o
-rw-rw-r-- 1 cyrus cyrus  184208 6月  28  2022 ./page_counter.o
-rw-rw-r-- 1 cyrus cyrus  141960 6月  28  2022 ./page_ext.o
-rw-rw-r-- 1 cyrus cyrus  302776 6月  28  2022 ./page_io.o
-rw-rw-r-- 1 cyrus cyrus  245424 6月  28  2022 ./page_isolation.o
-rw-rw-r-- 1 cyrus cyrus  238856 6月  28  2022 ./page_owner.o
-rw-rw-r-- 1 cyrus cyrus  184808 6月  28  2022 ./page_reporting.o
-rw-rw-r-- 1 cyrus cyrus  238872 6月  28  2022 ./page_vma_mapped.o
-rw-rw-r-- 1 cyrus cyrus  176296 6月  28  2022 ./pagewalk.o
-rw-rw-r-- 1 cyrus cyrus  723160 6月  28  2022 ./page-writeback.o
-rw-rw-r-- 1 cyrus cyrus  695184 6月  28  2022 ./percpu.o
-rw-rw-r-- 1 cyrus cyrus  253456 6月  28  2022 ./pgtable-generic.o
-rw-rw-r-- 1 cyrus cyrus  220320 6月  28  2022 ./process_vm_access.o
-rw-rw-r-- 1 cyrus cyrus  342752 6月  28  2022 ./readahead.o
-rw-rw-r-- 1 cyrus cyrus  560248 6月  28  2022 ./rmap.o
-rw-rw-r-- 1 cyrus cyrus  928272 6月  28  2022 ./shmem.o
-rw-rw-r-- 1 cyrus cyrus  156992 6月  28  2022 ./shuffle.o
-rw-rw-r-- 1 cyrus cyrus  638912 6月  28  2022 ./slab_common.o
-rw-rw-r-- 1 cyrus cyrus 1219240 6月  28  2022 ./slub.o
-rw-rw-r-- 1 cyrus cyrus  295952 6月  28  2022 ./sparse.o
-rw-rw-r-- 1 cyrus cyrus  232384 6月  28  2022 ./sparse-vmemmap.o
-rw-rw-r-- 1 cyrus cyrus  236184 6月  28  2022 ./swap_cgroup.o
-rw-rw-r-- 1 cyrus cyrus  758888 6月  28  2022 ./swapfile.o
-rw-rw-r-- 1 cyrus cyrus  827320 6月  28  2022 ./swap.o
-rw-rw-r-- 1 cyrus cyrus  252008 6月  28  2022 ./swap_slots.o
-rw-rw-r-- 1 cyrus cyrus  423008 6月  28  2022 ./swap_state.o
-rw-rw-r-- 1 cyrus cyrus  335368 6月  28  2022 ./truncate.o
-rw-rw-r-- 1 cyrus cyrus  155616 6月  28  2022 ./usercopy.o
-rw-rw-r-- 1 cyrus cyrus  338440 6月  28  2022 ./userfaultfd.o
-rw-rw-r-- 1 cyrus cyrus  356944 6月  28  2022 ./util.o
-rw-rw-r-- 1 cyrus cyrus  109120 6月  28  2022 ./vmacache.o
-rw-rw-r-- 1 cyrus cyrus  611632 6月  28  2022 ./vmalloc.o
-rw-rw-r-- 1 cyrus cyrus  239272 6月  28  2022 ./vmpressure.o
-rw-rw-r-- 1 cyrus cyrus 1891576 6月  28  2022 ./vmscan.o
-rw-rw-r-- 1 cyrus cyrus  559192 6月  28  2022 ./vmstat.o
-rw-rw-r-- 1 cyrus cyrus  350368 6月  28  2022 ./workingset.o
-rw-rw-r-- 1 cyrus cyrus  127552 6月  28  2022 ./zsmalloc.mod.o
-rw-rw-r-- 1 cyrus cyrus  536776 6月  28  2022 ./zsmalloc.o
