#ifndef _TEST_MEMBLOCK_H
#define _TEST_MEMBLOCK_H

void __init test_memblock(void);

static void __init test_memblock_alloc(void);
static void __init test_memblock_loopper(void);
static void __init test_memblock_dump(void);
static void __init test_memblock_memtest(void);

#endif