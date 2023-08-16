/**
 * @brief Distributed creation driver
 * @details
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#define COUNT 3
#define CNAME "mycdev"

struct class *cls;
int major = 500, minor = 0;
struct cdev *cdev;
struct device *dev;

static int __init myled_init(void)
{
    int ret, i;
    dev_t devno;
    printk(KERN_INFO "myled_init()\n");
    // 1. Allocate object
    cdev = cdev_alloc();
    if (cdev == NULL)
    {
        printk(KERN_ERR "cdev_alloc() fail\n");
        ret = -ENOMEM;
        goto ERR0;
    }
    // 2. Initialize cdev
    cdev_init(cdev, NULL);
    // 3. Register device number
    if (major > 0)
    {
        // 3.1 Static application
        ret = register_chrdev_region(MKDEV(major, minor), COUNT, CNAME);
        if (ret < 0)
        {
            printk(KERN_ERR "register_chrdev_region() fail\n");
            goto ERR1;
        }
        else
        {
            // 3.2 Dynamic application
            ret = alloc_chrdev_region(&devno, minor, COUNT, CNAME);
            if (ret < 0)
            {
                printk(KERN_ERR "alloc_chrdev_region() fail\n");
                goto ERR1;
            }
            major = MAJOR(devno);
            minor = MINOR(devno);
        }
        // 4. Registered character device driver
        ret = cdev_add(cdev, MKDEV(major, minor), COUNT);
        if (ret)
        {
            printk(KERN_ERR "cdev_add() fail\n");
            goto ERR2;
        }
        // 5. auto create device note
        cls = class_create(THIS_MODULE, CNAME);
        if (IS_ERR(cls))
        {
            printk(KERN_ERR "class_create() fail\n");
            ret = PTR_ERR(cls);
            goto ERR3;
        }
        for (i = 0; i < COUNT; i++)
        {
            dev = device_create(cls, NULL, MKDEV(major, minor + i), NULL, "%s%d", CNAME, i);
            if (IS_ERR(dev))
            {
                printk(KERN_ERR "device_create() fail\n");
                ret = PTR_ERR(dev);
                goto ERR4;
            }
        }
    }
    return 0;
ERR4:
    for (--i; i > 0; i--)
    {
        device_destroy(cls, MKDEV(major, minor + i));
    }
    class_destroy(cls);
ERR3:
    cdev_del(cdev);
ERR2:
    unregister_chrdev_region(MKDEV(major, minor), COUNT);
ERR1:
    kfree(cdev);
ERR0:
    return ret;
}
static void __exit myled_exit(void)
{
    int i;
    printk(KERN_INFO "myled_exit()\n");
    for (i = 0; i < COUNT; i++)
    {
        device_destroy(cls, MKDEV(major, minor + i));
    }
    class_destroy(cls);
    cdev_del(cdev);
    unregister_chrdev_region(MKDEV(major, minor), COUNT);
    kfree(cdev);
}

module_init(myled_init);
module_exit(myled_exit);
MODULE_LICENSE("GPL");