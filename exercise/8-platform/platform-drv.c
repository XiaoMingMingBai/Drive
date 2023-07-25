/**
 * @brief platform
 * @details 
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

int pdrv_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "pdev_probe()\n");
    // Your operations
    return 0;
}
int pdev_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "pdev_remove()\n");
    // resource release
    return 0;
}
static int __init platform_drv_init(void)
{
    printk(KERN_INFO "paltform_drv_init()\n");
    return 0;
}
static void __exit platform_drv_exit(void)
{
    printk(KERN_INFO "platform_drv_exit()\n");
}
module_init(platform_drv_init);
module_exit(platform_drv_exit);
MODULE_LICENSE("GPL");