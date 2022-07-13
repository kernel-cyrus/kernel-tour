#include <linux/init.h>
#include <linux/module.h>
#include <linux/pagewalk.h>

#include "../include/test-modules.h"

static int ptdump_gdb_entry(pgd_t *pgd, unsigned long addr,
			 unsigned long next, struct mm_walk *walk)
{
	pr_tour("PGD: %llx", pgd->pgd);
	return 0;
}

static int ptdump_pud_entry(pud_t *pud, unsigned long addr,
			 unsigned long next, struct mm_walk *walk)
{
	pr_tour("%llx PUD: %*c |- %llx", __pa(pud), 0, ' ', pud->pud);
	return 0;
}

static int ptdump_pmd_entry(pmd_t *pmd, unsigned long addr,
			 unsigned long next, struct mm_walk *walk)
{
	pr_tour("%llx PMD: %*c |- %llx", __pa(pmd), 4, ' ', pmd->pmd);
	return 0;
}

static int ptdump_pte_entry(pte_t *pte, unsigned long addr,
			 unsigned long next, struct mm_walk *walk)
{
	pr_tour("%llx PTE: %*c |- %llx", __pa(pte), 8, ' ', pte->pte);
	return 0;
}

static const struct mm_walk_ops walk_ops = {
	.pgd_entry	= ptdump_gdb_entry,
	.pud_entry	= ptdump_pud_entry,
	.pmd_entry	= ptdump_pmd_entry,
	.pte_entry	= ptdump_pte_entry
};

static int test_pagewalk_init(void)
{
	unsigned long start	= PAGE_OFFSET;
	unsigned long end	= ~0UL;

	pr_tour("Module init: test-pagewalk");

	pr_tour("test-pagewalk: dump from %lx ~ %lx", start, end);

	walk_page_range_novma(&init_mm, start, end, &walk_ops, NULL, NULL);

	pr_tour("test-pagewalk: done.");

	return 0;
}

static void test_pagewalk_exit(void)
{
	pr_tour("Module exit: test-pagewalk");
	
	return;
}

late_initcall(test_pagewalk_init);
module_exit(test_pagewalk_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");