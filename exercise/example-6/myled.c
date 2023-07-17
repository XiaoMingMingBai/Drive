#include "myled.h"
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#define CNAME "myled"
int major;
char kbuf[128] = {0};
unsigned int *rcc, *moder, *odr;

ssize_t myled_read(struct file *file, char __user *ubuf,
                   size_t size, loff_t *offs)
{
    int ret;
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    if (size > sizeof(kbuf))
    {
        size = sizeof(kbuf);
    }
    ret = copy_to_user(ubuf, kbuf, size);
    if (ret)
    {
        printk("copy_to_user error\n");
        return -EIO;
    }
    return size;
}
ssize_t myled_write(struct file *file, const char __user *ubuf,
                    size_t size, loff_t *offs)
{
    int ret;
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    if (size > sizeof(kbuf))
    {
        size = sizeof(kbuf);
    }
    ret = copy_from_user(kbuf, ubuf, size);
    if (ret)
    {
        printk("copy_from_user error\n");
        return -EIO;
    }
    kbuf[0] == 1 ? (*odr |= (1 << 10)) : (*odr &= (~(1 << 10)));
    return size;
}
int myled_open(struct inode *inode, struct file *file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    // 1. Address mapping.
    rcc = ioremap(RCC_MP_AHB4ENSETR, 4);
    if (rcc == NULL)
    {
        printk("ioremap error\n");
        return -ENOMEM;
    }
    moder = ioremap(GPIOE_MODER, 4);
    if (moder == NULL)
    {
        printk("ioremap error\n");
        return -ENOMEM;
    }
    odr = ioremap(GPIOE_ODR, 4);
    if (odr == NULL)
    {
        printk("ioremap error\n");
        return -ENOMEM;
    }
    // 2. Led initialization.
    *rcc |= (1 << 4);
    *moder &= ~(3 << 20);
    *moder |= (1 << 20);
    *odr &= ~(1 << 10);
    return 0;
}
int myled_release(struct inode *inode, struct file *file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    iounmap(rcc);
    iounmap(moder);
    iounmap(odr);
    return 0;
}
const struct file_operations fops = {
    .read = myled_read,
    .write = myled_write,
    .open = myled_open,
    .release = myled_release,
};

static int __init myled_init(void)
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
static void __exit myled_exit(void)
{
    unregister_chrdev(major, CNAME);
    printk("unregister_chrdev success, major = %d\n", major);
}
module_init(myled_init);
module_exit(myled_exit);

MODULE_LICENSE("GPL");