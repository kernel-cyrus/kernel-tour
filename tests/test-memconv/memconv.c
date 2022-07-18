#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <asm/memory.h>
#include <asm/fixmap.h>
#include <asm/kasan.h>
#include <asm/pgtable.h>

#include "../include/test-modules.h"

#include "memconv.h"

static int test_memconv_init(void)
{
	pr_tour("Module init: test-memconv");

	print_vm_layout();

	test_memconv();

	return 0;
}

static void test_memconv_exit(void)
{
	pr_tour("Module exit: test-memconv");
	return;
}

static void test_memconv(void)
{
	phys_addr_t phys = virt_to_phys(&swapper_pg_dir);
	void *p_linear = phys_to_virt(phys);
	struct page* p_page = virt_to_page(p_linear);

	// Buildtime virtual address of a symbol in kernel image.
	pr_tour("vaddr: %p", &swapper_pg_dir);

	// Get physical address
	// __pa will print warnings when CONFIG_DEBUG_VIRTUAL is on.
	pr_tour("virt_to_phys: %llx", phys);
	pr_tour("__pa: %llx", __pa(swapper_pg_dir));
	pr_tour("__pa_symbol: %llx", __pa_symbol(swapper_pg_dir));

	// Get Linear virtual address of the paddr.
	pr_tour("__va: %p", __va(phys));
	pr_tour("phys_to_virt: %p", phys_to_virt(phys));

	// Get pfn from both kernel vaddr and linear vaddr, should be the same.
	pr_tour("kernel vaddr - virt_to_pfn: %ld", virt_to_pfn(&swapper_pg_dir));
	pr_tour("linear vaddr - virt_to_pfn: %ld", virt_to_pfn(p_linear));

	// The page struct of vaddr, only linear vaddr supported.
	pr_tour("kernel vaddr - virt_addr_valid: %d", virt_addr_valid(&swapper_pg_dir));
	pr_tour("kernel vaddr - virt_to_page: %p (WRONG!)", virt_to_page(&swapper_pg_dir));
	pr_tour("kernel vaddr - page_to_virt: %p (WRONG!)", page_to_virt(virt_to_page(&swapper_pg_dir))); // to "kernel" vaddr
	pr_tour("linear vaddr - virt_addr_valid: %d", virt_addr_valid(p_linear));
	pr_tour("linear vaddr - virt_to_page: %p", virt_to_page(p_linear));
	pr_tour("linear vaddr - page_to_virt: %p", page_to_virt(virt_to_page(p_linear)));

	// The page struct of paddr
	pr_tour("phys_to_page: %p", phys_to_page(phys));
	pr_tour("page_to_phys: %llx", page_to_phys(p_page));

	// The page struct of paddr
	pr_tour("page_to_pfn: %ld", page_to_pfn(phys_to_page(phys)));
	pr_tour("pfn_to_page: %p", pfn_to_page(virt_to_pfn(p_linear)));
}

static void print_vm_layout(void)
{
	pr_tour("VM Layout:");
	pr_tour("Linear Mapping: %lx ~ %lx", PAGE_OFFSET, PAGE_END);
#if defined(CONFIG_KASAN_GENERIC) || defined(CONFIG_KASAN_SW_TAGS)
	pr_tour("Kasan Shadow:   %lx ~ %lx", KASAN_SHADOW_START, KASAN_SHADOW_END);
#endif
	pr_tour("Modules:        %lx ~ %lx", MODULES_VADDR, MODULES_END);
	pr_tour("Vmalloc:        %lx ~ %lx", VMALLOC_START, VMALLOC_END);
	pr_tour("Fixmap:         %lx ~ %lx", FIXADDR_START, FIXADDR_TOP);
	pr_tour("PCI IO:         %lx ~ %lx", PCI_IO_START, PCI_IO_END);
	pr_tour("vmemmap:        %lx ~ %lx", VMEMMAP_START, VMEMMAP_START + VMEMMAP_SIZE);
}

late_initcall(test_memconv_init);
module_exit(test_memconv_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");