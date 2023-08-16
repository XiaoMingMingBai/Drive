/**
 * This file registers a character device driver and defines the open/read/write/close functions.
 * To use it you need to manually create character device files:
 *      sudo mknod /dev/mycdev c 235 0
 *          mknod : Create a device node command.
 *          /dev/mycdev : Create the path (any) and name of the device node.
 *          c/b : c Character Device b block device.
 *          235 : Master device number.
 *          0 : Secondary device number.
*/      
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#define CNAME "mycdev"
int major;
int mycdev_open(struct inode *inode, struct file *file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
ssize_t mycdev_read(struct file *file,
                    char __user *ubuf, size_t size, loff_t *offs)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
ssize_t mycdev_write(struct file *file,
                     const char __user *ubuf, size_t size, loff_t *offs)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
int mycdev_close(struct inode *inode, struct file *file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
const struct file_operations fops = {
    .open = mycdev_open,
    .read = mycdev_read,
    .write = mycdev_write,
    .release = mycdev_close,
};
static int __init mycdev_init(void)
{
    // 1. Register character device drivers.
    major = register_chrdev(0, CNAME, &fops);
    if (major < 0)
    {
        printk("register_chrdev error\n");
        return major;
    }
    printk("chrdev success,major = %d\n", major);
    return 0;
}
static void __exit mycdev_exit(void)
{
    // 2. Deregister the character device driver
    unregister_chrdev(major, CNAME);
}
module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");