/**
 * @brief create 3 devices
 * @details Create 3 led drivers to control each of the three lights
*/
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#define COUNT 3
#define CNAME "mycdev"
int major = 500;
int minor = 0;
char kbuf[128] = { 0 };
struct cdev* cdev;
struct class* cls;
struct device* dev;
int curminor;
int mycdev_open(struct inode* inode, struct file* file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    curminor = MINOR(inode->i_rdev);
    return 0;
}
ssize_t mycdev_read(struct file* file,
    char __user* ubuf, size_t size, loff_t* offs)
{
    int ret;
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    if (size > sizeof(kbuf))
        size = sizeof(kbuf);
    ret = copy_to_user(ubuf, kbuf, size);
    if (ret) {
        printk("copy_from_user error\n");
        return -EIO;
    }
    return size;
}
ssize_t mycdev_write(struct file* file,
    const char __user* ubuf, size_t size, loff_t* offs)
{
    int ret;
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    if (size > sizeof(kbuf))
        size = sizeof(kbuf);
    ret = copy_from_user(kbuf, ubuf, size);
    if (ret) {
        printk("copy_from_user error\n");
        return -EIO;
    }
    switch (curminor) {
    case 0:
        kbuf[0] == '1' ? printk("led1 on\n") : printk("led1 off\n");
        break;
    case 1:
        kbuf[0] == '1' ? printk("led2 on\n") : printk("led2 off\n");
        break;
    case 2:
        kbuf[0] == '1' ? printk("led3 on\n") : printk("led3 off\n");
        break;
    }
    return size;
}
int mycdev_close(struct inode* inode, struct file* file)
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
    int ret, i;
    dev_t devno;
    // 1.分配对象cdev
    cdev = cdev_alloc();
    if (cdev == NULL) {
        printk("cdev_alloc error\n");
        ret = -ENOMEM;
        goto ERR1;
    }
    // 2.初始化对象
    cdev_init(cdev, &fops);
    // 3.申请设备号
    if (major > 0) {
        // 3.1静态申请
        ret = register_chrdev_region(MKDEV(major, minor), COUNT, CNAME);
        if (ret) {
            printk("register_chrdev_region error\n");
            goto ERR2;
        }
    } else {
        // 3.2动态申请
        ret = alloc_chrdev_region(&devno, minor, COUNT, CNAME);
        if (ret) {
            printk("alloc_chrdev_region error\n");
            goto ERR2;
        }
        major = MAJOR(devno);
        minor = MINOR(devno);
    }
    // 4.注册字符设备驱动
    ret = cdev_add(cdev, MKDEV(major, minor), COUNT);
    if (ret) {
        printk("cdev_add error\n");
        goto ERR3;
    }
    // 5.自动创建设备节点
    cls = class_create(THIS_MODULE, CNAME);
    if (IS_ERR(cls)) {
        printk("class_create error\n");
        ret = PTR_ERR(cls);
        goto ERR4;
    }
    for (i = 0; i < COUNT; i++) {
        dev = device_create(cls, NULL, MKDEV(major, minor + i), NULL, "%s%d", CNAME, i);
        if (IS_ERR(dev)) {
            printk("device_create error\n");
            ret = PTR_ERR(dev);
            goto ERR5;
        }
    }
    return 0;
ERR5:
    for (--i; i >= 0; i--) {
        device_destroy(cls, MKDEV(major, minor + i));
    }
    class_destroy(cls);
ERR4:
    cdev_del(cdev);
ERR3:
    unregister_chrdev_region(MKDEV(major, minor), COUNT);
ERR2:
    kfree(cdev);
ERR1:
    return ret;
}
static void __exit mycdev_exit(void)
{
    int i;
    for (i = 0; i < COUNT; i++) {
        device_destroy(cls, MKDEV(major, minor + i));
    }
    class_destroy(cls);
    cdev_del(cdev);
    unregister_chrdev_region(MKDEV(major, minor), COUNT);
    kfree(cdev);
}
module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");