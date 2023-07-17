/**
 * This file registers a device driver file and defines the open/read/write/release functions.
 * This is done with a spatial copy in the read/write function.
 * You also need to manually create the device driver file:
 *      sudo mknod dev_path/name dev_file_type major_num minor_num
*/
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#define CNAME "mycdev"
int major;
char kbuf[128] = {0};

int mycdev_open(struct inode *inode, struct file *file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
ssize_t mycdev_read(struct file *file, char __user *ubuf,
                    size_t size, loff_t *offs)
{
    int ret;
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    if (size > sizeof(kbuf))
    {
        size = sizeof(kbuf);
    }
    // Copy kernel space data to user space.
    ret = copy_to_user(ubuf, kbuf, size);
    if (ret) // If the number of failures is returned, it is a copy error.
    {
        printk("copy_to_user error\n");
        return -EIO;
    }
    return size;
}
ssize_t mycdev_write(struct file *file, const char __user *ubuf,
                     size_t size, loff_t *offs)
{
    int ret;
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    if (size > sizeof(kbuf))
    {
        size = sizeof(kbuf);
    }
    // Copy user space data to kernel space.
    ret = copy_from_user(kbuf, ubuf, size);
    if (ret) // If the number of failures is returned, it is a copy error.
    {
        printk("copy_from_user error\n");
        return -EIO;
    }
    return size;
}
int mycdev_release(struct inode *inode, struct file *file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
// Define a file operation structure and assign a value.
const struct file_operations fops = {
    .open = mycdev_open,
    .read = mycdev_read,
    .write = mycdev_write,
    .release = mycdev_release,
};

static int __init mycdev_init(void)
{
    major = register_chrdev(0, CNAME, &fops);
    if (major < 0)
    {
        printk("register_chrdev error\n");
        return major;
    }
    printk("register_chrdev success, major = %d\n", major);
    return 0;
}
static void __exit mycdev_exit(void)
{
    unregister_chrdev(major, CNAME);
    printk("unregister_chrdev success, major = %d\n", major);
}
module_init(mycdev_init);
module_exit(mycdev_exit);

MODULE_LICENSE("GPL");