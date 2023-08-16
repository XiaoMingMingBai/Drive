/**
 * This file defines a function and exports the symbol table.
*/
#include <linux/init.h>
#include <linux/module.h>

int add(int a,int b)
{
    return (a+b);
}
EXPORT_SYMBOL_GPL(add); // Export symbol table.
static int __init demoA_init(void)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);    
    return 0;
}
static void __exit demoA_exit(void)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
}
module_init(demoA_init);
module_exit(demoA_exit);
MODULE_LICENSE("GPL");