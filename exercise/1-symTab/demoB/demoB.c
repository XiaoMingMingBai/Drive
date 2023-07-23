/**
 * This file declares an external function and uses it.
 * The symbol table path must be specified at make:
 *      KBUILD_EXTRA_SYMBOLS += <symbol_table_path>
*/
#include <linux/init.h>
#include <linux/module.h>
extern int add(int ,int); // Declare external functions
static int __init demoB_init(void)
{
    printk("%s:sum = %d\n",__func__,add(100,200));
    return 0;
}
static void __exit demoB_exit(void)
{
}
module_init(demoB_init);
module_exit(demoB_exit);
MODULE_LICENSE("GPL");