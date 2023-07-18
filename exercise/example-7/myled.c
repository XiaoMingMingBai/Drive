#include "myled.h"
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#define CNAME "myled"
int major;
char kbuf[128] = {0};
unsigned int *rcc;
gpio_t *gpioe, *gpiof;
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
void led_set_value(enum led_num which, enum led_status status)
{
    switch (which)
    {
    case LED1:
        status == ON ? (gpioe->ODR |= (1 << 10)) : (gpioe->ODR &= ~(1 << 10));
        break;
    case LED2:
        status == ON ? (gpiof->ODR |= (1 << 10)) : (gpiof->ODR &= ~(1 << 10));
        break;
    case LED3:
        status == ON ? (gpioe->ODR |= (1 << 8)) : (gpioe->ODR &= ~(1 << 8));
        break;
    }
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
    led_set_value(kbuf[0], kbuf[1]);
    return size;
}
int myled_open(struct inode *inode, struct file *file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    // 1. Mapping address
    rcc = ioremap(RCC_MP_AHB4ENSETR, 4);
    if (rcc == NULL)
    {
        printk("ioremap error\n");
        return -ENOMEM; // Out of memory
    }
    gpioe = ioremap(GPIOE_BASE, sizeof(gpio_t));
    if (gpioe == NULL)
    {
        printk("ioremap error\n");
        return -ENOMEM; // Out of memory
    }
    gpiof = ioremap(GPIOF_BASE, sizeof(gpio_t));
    if (gpiof == NULL)
    {
        printk("ioremap error\n");
        return -ENOMEM; // Out of memory
    }
    // 2. led initialization
    *rcc |= (3 << 4);
    // 2.1 LED1
    gpioe->MODER &= ~(3 << 20);
    gpioe->MODER |= (1 << 20);
    gpioe->ODR &= ~(1 << 10);
    // 2.2 LED2
    gpiof->MODER &= ~(3 << 20);
    gpiof->MODER |= (1 << 20);
    gpiof->ODR &= ~(1 << 10);
    // 2.3 LED3
    gpioe->MODER &= ~(3 << 16);
    gpioe->MODER |= (1 << 16);
    gpioe->ODR &= ~(1 << 8);
    return 0;
}
int myled_release(struct inode *inode, struct file *file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    // 3. Unmap
    iounmap(rcc);
    iounmap(gpioe);
    iounmap(gpiof);
    return 0;
}
const struct file_operations fops =
{
    .open = myled_open,
    .read = myled_read,
    .write = myled_write,
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