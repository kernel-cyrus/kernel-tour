#include <linux/init.h>
#include <linux/module.h>
#include <linux/memblock.h>
#include <linux/cache.h>

#include "../include/test-modules.h"
#include "memblock.h"

/*
 *  To run this test:
 *  1. Apply the patch to kernel source
 *  2. Append "memblock=debug memtest" to bootargs
 */

void __init test_memblock(void)
{
	test_memblock_alloc();

	test_memblock_loopper();

	test_memblock_dump();

	test_memblock_memtest();
}

static void __init test_memblock_alloc(void)
{
	char *buf;

	buf = memblock_alloc(64, SMP_CACHE_BYTES);
	if (WARN_ON(!buf))
		return;
	sprintf(buf, "Hello memblock~");
	pr_tour("test-memblock: buf: %p %s", (void*)buf, buf);
}

static void __init test_memblock_loopper(void)
{
	u64 i;
	phys_addr_t start, end;

	for_each_reserved_mem_range(i, &start, &end)
		pr_tour("Reserved: %llx~%llx", start, end);
}

static void __init test_memblock_dump(void)
{
	memblock_dump_all();
}

static void __init test_memblock_memtest(void)
{
	phys_addr_t start, end;

	start = memblock_start_of_DRAM();
	end = memblock_end_of_DRAM();

	parse_memtest(NULL);
	early_memtest(start, end);
}