/**
 * This is the basic kernel driven code.
 * It shows the use of the printk function.
*/
#include <linux/init.h>
#include <linux/module.h>

static int __init demo_init(void)
{
    printk(KERN_ERR "init:this is test pritk usage...\n");
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
static void __exit demo_exit(void)
{
    printk(KERN_ERR "exit:this is test pritk usage###\n");
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
}

module_init(demo_init); // Statement installation function.
module_exit(demo_exit); // Statement unload function.


MODULE_LICENSE("GPL"); // Designation license.
MODULE_DESCRIPTION("this first devers."); // Designation description.
MODULE_ALIAS("cdev"); // Designation alias.
MODULE_AUTHOR("ming <xiaomingmingbai@outlook.com>"); // Designation author.