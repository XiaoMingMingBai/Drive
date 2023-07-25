/**
 * @brief platform
 * @details 
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

struct resource res[] = {
    [0] = {
        .start = 0x12345678,
        .end = 0x12345678 + 49,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = 62,
        .end = 62,
        .flags = IORESOURCE_IRQ,
    },
};
void pdev_release(struct device *dev)
{
    printk(KERN_INFO "pdev_release()\n");
}
struct platform_device pdev = {
    .name = "xiaomingmingbai",
    .id = PLATFORM_DEVID_AUTO,
    .dev = {
        .release = pdev_release,
    },
    .resource = res,
    .num_resources = ARRAY_SIZE(res),
};
static int __init platform_dev_init(void)
{
    int ret;
    printk(KERN_INFO "platform_dev_init()\n");
    ret = platform_device_register(&pdev);
    if (ret)
    {
        printk(KERN_ERR "platform_device_register() fail\n");
        return -EBUSY;
    }
    return ret;
}
static void __exit platform_dev_exit(void)
{
    printk(KERN_INFO "platform_dev_exit()\n");
    platform_device_unregister(&pdev);
}
module_init(platform_dev_init);
module_exit(platform_dev_exit);
MODULE_LICENSE("GPL");