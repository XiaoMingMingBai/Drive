/**
 * @brief IIC reads the version number
 * @details The read and write encapsulation is complete, and the version
 *          information is successfully obtained
 */
#include "si7006.h"
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#define CNAME "si7006"
struct i2c_client *gclient;
int major;
struct class *cls;
struct device *dev;

int i2c_read_serial_firmware(unsigned short reg)
{
    int ret;
    unsigned char data;
    char buf[2] = {(reg >> 8), (reg & 0xff)};
    // 1. 封装数据
    struct i2c_msg msgs[] =
    {
        [0] = {
            .addr = gclient->addr,
            .flags = 0,
            .len = 2,
            .buf = buf,
        },
        [1] = {
            .addr = gclient->addr,
            .flags = I2C_M_RD,
            .len = 1,
            .buf = &data,
        },
    };
    // 2. 发送数据
    ret = i2c_transfer(gclient->adapter, msgs, ARRAY_SIZE(msgs));
    if (ret != ARRAY_SIZE(msgs))
    {
        printk(KERN_INFO "i2c_transfer fail\n");
        return EAGAIN;
    }
    return data;
}


// ioctl
long si7006_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int data, ret;
    switch (cmd)
    {
    case GET_SERIAL:
        data = i2c_read_serial_firmware(SERIAL_ADDR);
        if (data < 0)
        {
            printk(KERN_INFO "i2c_read_serial_firmware fail\n");
            return EAGAIN;
        }
        data = data & 0xff;
        ret = copy_to_user((void *)arg, &data, sizeof(data));
        if (ret < 0)
        {
            printk(KERN_INFO "copy_to_user fail\n");
            return EAGAIN;
        }
        break;
    case GET_FIRMWARE:
        data = i2c_read_serial_firmware(FIRMWARE_ADDR);
        if (data < 0)
        {
            printk(KERN_INFO "i2c_read_serial_firmware fail\n");
            return EAGAIN;
        }
        data = data & 0xff;
        ret = copy_to_user((void *)arg, &data, sizeof(data));
        if (data < 0)
        {
            printk(KERN_INFO "i2c_read_serial_firmware fail\n");
            return EAGAIN;
        }
        break;
    }
    return 0;
}
// open
int si7006_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "si7006 open\n");
    return 0;
}
// close
int si7006_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "si7006 release\n");
    return 0;
}
struct file_operations fops = {
    .open = si7006_open,
    .release = si7006_release,
    .unlocked_ioctl = si7006_unlocked_ioctl,
};
struct of_device_id si7006_of_match[] = {
    {.compatible = "ming,si7006"},
    {},
};
int si7006_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    gclient = client;
    printk(KERN_INFO "si7006 probe\n");
    // 1. Register char device driver
    major = register_chrdev(0, CNAME, &fops);
    if (major < 0)
    {
        printk(KERN_INFO "register_chrdev fail\n");
        return major;
    }
    // 2. Create device node
    cls = class_create(THIS_MODULE, CNAME);
    if (IS_ERR(cls))
    {
        printk(KERN_INFO "class_create fail\n");
        return PTR_ERR(cls);
    }
    dev = device_create(cls, NULL, MKDEV(major, 0), NULL, CNAME);
    if (IS_ERR(dev))
    {
        printk(KERN_INFO "device_create fail\n");
        return PTR_ERR(dev);
    }
    return 0;
}
int si7006_remove(struct i2c_client *client)
{
    printk(KERN_INFO "si7006 remove\n");
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, CNAME);
    return 0;
}
struct i2c_driver si7006 = {
    .driver = {
        .name = "si7006",
        .of_match_table = si7006_of_match,
    },
    .probe = si7006_probe,
    .remove = si7006_remove,
};

module_i2c_driver(si7006);
MODULE_LICENSE("GPL");