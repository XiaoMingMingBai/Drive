/**
 * @brief ioctrl
 * @details The structure is passed through ioctrl.
*/
#include "ioctrlStruct.h"
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#define CNAME "myled"

int major;
char kbuf[128] = { 0 };
unsigned int* rcc;
gpio_t *gpioe, *gpiof;
int myled_open(struct inode* inode, struct file* file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    rcc = ioremap(RCC_MP_AHB4ENSETR, 4);
    if (rcc == NULL) {
        printk("ioremap error\n");
        return -ENOMEM;
    }
    gpioe = ioremap(GPIOE_BASE, sizeof(gpio_t));
    if (gpioe == NULL) {
        printk("ioremap error\n");
        return -ENOMEM;
    }
    gpiof = ioremap(GPIOF_BASE, sizeof(gpio_t));
    if (gpiof == NULL) {
        printk("ioremap error\n");
        return -ENOMEM;
    }
    *rcc |= (1 << 4) | (1 << 5);
    gpioe->MODER &= ~(3 << 20);
    gpioe->MODER |= (1 << 20);
    gpioe->ODR &= ~(1 << 10);
    gpiof->MODER &= ~(3 << 20);
    gpiof->MODER |= (1 << 20);
    gpiof->ODR &= ~(1 << 10);
    gpioe->MODER &= ~(3 << 16);
    gpioe->MODER |= (1 << 16);
    gpioe->ODR &= ~(1 << 8);

    return 0;
}
ssize_t myled_read(struct file* file,
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
void led_set_value(int which, int status)
{
    switch (which) {
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
ssize_t myled_write(struct file* file,
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
    return size;
}
long myled_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{
    img_t img;
    int which, ret;
    switch (cmd) {
    case LED_ON:
        ret = copy_from_user(&which, (void*)arg, GET_CMD_SIZE(LED_ON));
        if (ret) {
            printk("copy_from_user error\n");
            return -EIO;
        }
        printk("which = %d,ON=%d\n", which, ON);
        led_set_value(which, ON);
        break;
    case LED_OFF:
        ret = copy_from_user(&which, (void*)arg, GET_CMD_SIZE(LED_OFF));
        if (ret) {
            printk("copy_from_user error\n");
            return -EIO;
        }
        printk("which = %d,OFF=%d\n", which, OFF);
        led_set_value(which, OFF);
        break;
    case UACCESS_CHAR_ARRAY:
        ret = copy_from_user(kbuf, (void*)arg, GET_CMD_SIZE(UACCESS_CHAR_ARRAY));
        if (ret) {
            printk("copy_from_user error\n");
            return -EIO;
        }
        printk("kbuf = %s\n", kbuf);
        break;
    case UACCESS_STRUCT:
        ret = copy_from_user(&img, (void*)arg, GET_CMD_SIZE(UACCESS_STRUCT));
        if (ret) {
            printk("copy_from_user error\n");
            return -EIO;
        }
        printk("img.width = %d,img.high = %d\n", img.width, img.high);
        img.width = 1100;
        img.high = 800;
        ret = copy_to_user((void*)arg, &img, GET_CMD_SIZE(UACCESS_STRUCT));
        if (ret) {
            printk("copy_from_user error\n");
            return -EIO;
        }
        break;
    }
    return 0;
}
int myled_close(struct inode* inode, struct file* file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    iounmap(rcc);
    iounmap(gpiof);
    iounmap(gpioe);
    return 0;
}
const struct file_operations fops = {
    .open = myled_open,
    .read = myled_read,
    .write = myled_write,
    .unlocked_ioctl = myled_ioctl,
    .release = myled_close,

};
static int __init myled_init(void)
{
    major = register_chrdev(0, CNAME, &fops);
    if (major < 0) {
        printk("register_chrdev error\n");
        return major;
    }
    printk("chrdev success,major = %d\n", major);
    return 0;
}
static void __exit myled_exit(void)
{
    unregister_chrdev(major, CNAME);
}
module_init(myled_init);
module_exit(myled_exit);
MODULE_LICENSE("GPL");