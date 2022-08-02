#include <linux/init.h>
#include <linux/module.h>
#include <linux/rculist.h>
#include <linux/list.h>

#include "../include/test-modules.h"

struct rcu_node {
	struct list_head	rcu_list;
	int			value;
};

LIST_HEAD(rcu_node);

static void print_list(void)
{
	struct rcu_node *p_node = NULL;

	rcu_read_lock();	// declare read-side critical section
	list_for_each_entry_rcu(p_node, &rcu_node, rcu_list) {
		pr_tour("Node: %d", p_node->value);
	}
	rcu_read_unlock();	// reader release
	pr_tour("------");
}

static int test_rcu_init(void)
{
	struct rcu_node *node_0 = NULL;
	struct rcu_node *node_1 = NULL;
	struct rcu_node *node_2 = NULL;
	struct rcu_node *p_node = NULL;

	pr_tour("Module init: test-rcu");

	node_0 = kzalloc(sizeof(*node_0), GFP_KERNEL);
	if (!node_0)
		goto exit;
	node_0->value = 0;

	node_1 = kzalloc(sizeof(*node_1), GFP_KERNEL);
	if (!node_1)
		goto exit;
	node_1->value = 1;

	node_2 = kzalloc(sizeof(*node_2), GFP_KERNEL);
	if (!node_2)
		goto exit;
	node_2->value = 2;

	INIT_LIST_HEAD_RCU(&rcu_node);

	list_add_tail_rcu(&node_0->rcu_list, &rcu_node);
	list_add_tail_rcu(&node_1->rcu_list, &rcu_node);
	list_add_tail_rcu(&node_2->rcu_list, &rcu_node);

	// Iterate list node
	print_list();

	// Replace list with new copy node (Update)
	p_node = kzalloc(sizeof(*p_node), GFP_KERNEL);
	*p_node = *node_0;
	p_node->value = 8;
	list_replace_rcu(&node_0->rcu_list, &p_node->rcu_list);
	synchronize_rcu();	// wait for all readers relased
	kfree(node_0);		// free the original node
	node_0 = NULL;		// avoid double free in exit section

	// Iterate list node
	print_list();

	// Remove list node
	list_del_rcu(&p_node->rcu_list);
	synchronize_rcu();	// wait for all readers relased

	// Iterate list node
	print_list();

exit:
	if(node_0)
		kfree(node_0);
	if(node_1)
		kfree(node_1);
	if(node_2)
		kfree(node_2);
	if(p_node)
		kfree(p_node);

	return 0;
}

static void test_rcu_exit(void)
{
	pr_tour("Module exit: test-rcu");
	return;
}

late_initcall(test_rcu_init);
module_exit(test_rcu_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");