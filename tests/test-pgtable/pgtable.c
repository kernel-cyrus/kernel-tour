#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <asm/pgtable-types.h>
#include <asm/pgtable.h>

#include "../include/test-modules.h"

static int test_pgtable_init(void)
{
	int i;
	pgd_t pgd;

	pr_tour("Module init: test-pgtable");

	pr_tour("Test the size of pte_t, pmd_t, pud_t, pgd_t: %ld", sizeof(pte_t));
	
	pr_tour("VA Addr of swapper_pg_dir: 0x%p", &swapper_pg_dir);

	pr_tour("PA Addr of swapper_pg_dir: 0x%llx", __pa_symbol(swapper_pg_dir));

	pr_tour("Size of swapper_pg_dir: %d", PTRS_PER_PGD);

	for (i = 1; i < PTRS_PER_PGD; i++) {
		pgd = swapper_pg_dir[i];
		if (!pgd_val(pgd))
			continue;
		pr_tour("PGD[%d]: 0x%llx, paddr: 0x%llx, vaddr: 0x%p", i,
			pgd_val(pgd), __pgd_to_phys(pgd), __va(__pgd_to_phys(pgd)));

	}

	return 0;
}

static void test_pgtable_exit(void)
{
	pr_tour("Module exit: test-pgtable");
	
	return;
}

late_initcall(test_pgtable_init);
module_exit(test_pgtable_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");