/**
 * @brief auto create cdev
 * @details 
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#define CNAME "myled"

int major;
struct class *cls;
struct device *dev;

const struct file_operations fops = {};
static int __init myled_init(void)
{
    int ret;
    major = register_chrdev(0, CNAME, &fops);
    if (major < 0)
    {
        printk(KERN_ERR "register_chrdev failed\n");
        ret = major;
        goto ERR0;
    }
    printk(KERN_INFO "major = %d\n", major);
    cls = class_create(THIS_MODULE, CNAME);
    if(IS_ERR(cls)){
        printk(KERN_ERR "class_create failed\n");
        ret = PTR_ERR(cls);
        goto ERR1;
    }
    dev = device_create(cls, NULL, MKDEV(major, 0), NULL, CNAME);
    if(IS_ERR(dev))
    {
        printk(KERN_ERR "device_create failed\n");
        ret = PTR_ERR(dev);
        goto ERR2;
    }
    return 0;
ERR2:
    class_destroy(cls);
ERR1:
    unregister_chrdev(major, CNAME);
ERR0:
    return ret;
}
static void __exit myled_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, CNAME);
}
module_init(myled_init);
module_exit(myled_exit);
MODULE_LICENSE("GPL");